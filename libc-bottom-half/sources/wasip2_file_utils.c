#include <assert.h>
#include <common/errors.h>
#include <errno.h>
#include <stddef.h>
#include <string.h>
#include <wasi/file_utils.h>

#ifdef __wasip2__

/**
 * Validates that `ptr_signed` is a valid utf-8 string.
 *
 * Returns the byte-length of the string on success, or -1 if it's invalid
 * utf-8.
 */
static int validate_utf8(const char *ptr_signed) {
  const unsigned char *ptr = (const unsigned char *)ptr_signed;
  size_t i = 0;
  while (1) {
    unsigned char c = ptr[i];
    size_t codepoint_size;

    if (c == 0x00) {
      break;
    } else if (c <= 0x7F) {
      // fast path for ascii-like characters, just turn the loop.
      i++;
      continue;
    } else if ((c & 0xE0) == 0xC0) {
      codepoint_size = 2;
      // Disallow overlong encodings, which in this case is either 0xc0 or
      // 0xc1. All other patterns represent a valid encoding.
      if (c <= 0xC1)
        return -1;
    } else if ((c & 0xF0) == 0xE0) {
      codepoint_size = 3;
      // Disallow overlong encodings by ensuring that if no bits are set in
      // this byte then the highest payload bit in the next byte must be set.
      if (c == 0xE0 && (ptr[i + 1] & 0x20) == 0x00)
        return -1;

      // Disallow surrogates which start with 0xED and are followed with
      // something bigger than 0xA0.
      if (c == 0xED && ptr[i + 1] >= 0xA0)
        return -1;
    } else if ((c & 0xF8) == 0xF0) {
      codepoint_size = 4;

      // Disallow overlong encodings by ensuring that if no payload bits are
      // set in this byte then one of the upper two payload bits in the next
      // byte are set.
      if (c == 0xF0 && (ptr[i + 1] & 0x30) == 0x00)
        return -1;

      // Disallow codepoints beyond U+10FFFF which start with 0xF4 and are
      // followed with something bigger than 0x8F, or if the leading byte is
      // larger than 0xF4.
      if (c == 0xF4 && ptr[i + 1] >= 0x90)
        return -1;
      if (c > 0xF4)
        return -1;
    } else {
      return -1;
    }

    // Validate all continuation bytes are `0b10......`
    for (size_t j = 1; j < codepoint_size; j++)
      if ((ptr[i + j] & 0xC0) != 0x80)
        return -1;
    i += codepoint_size;
  }
  return i;
}

int wasip2_string_from_c(const char *s, wasip2_string_t *out) {
  int len = validate_utf8(s);
  if (len < 0) {
    errno = EILSEQ;
    return -1;
  }
  out->ptr = (uint8_t *)s;
  out->len = len;
  return 0;
}

ssize_t __wasilibc_write(wasip2_write_t *write, const void *buffer,
                         size_t length) {
  assert(write->output.__handle != 0);
  while (true) {
    streams_stream_error_t error;
    uint64_t count;

    // See how many bytes can be written to this stream, if any.
    if (!streams_method_output_stream_check_write(write->output, &count,
                                                  &error))
      return wasip2_handle_write_error(error);

    if (count) {
      // If bytes can be written to the stream, then attempt the write.
      count = count < length ? count : length;
      wasip2_list_u8_t list = {.ptr = (uint8_t *)buffer, .len = count};
      if (!streams_method_output_stream_write(write->output, &list, &error))
        return wasip2_handle_write_error(error);

      // For blocking writes additionally perform a blocking flush to ensure
      // that the data makes its way to the destination.
      if (write->blocking) {
        bool ok =
            streams_method_output_stream_blocking_flush(write->output, &error);
        if (!ok)
          return wasip2_handle_write_error(error);
      }

      // Update the offset if this stream is tracking that.
      if (write->offset)
        *write->offset += count;

      return count;
    }

    // This stream isn't currently writable, and this is a non-blocking
    // operation, so bail out.
    if (!write->blocking) {
      errno = EWOULDBLOCK;
      return -1;
    }

    // Lazily initialize the pollable if one hasn't already been created yet.
    if (write->pollable->__handle == 0)
      *write->pollable = streams_method_output_stream_subscribe(write->output);
    poll_borrow_pollable_t pollable_borrow =
        poll_borrow_pollable(*write->pollable);

    // Either wait for a timeout or indefinitely for this stream to become
    // writable. Once this is done loop around back to the beginning.
    if (write->timeout != 0) {
      monotonic_clock_own_pollable_t timeout_pollable =
          monotonic_clock_subscribe_duration(write->timeout);
      poll_list_borrow_pollable_t pollables;
      poll_borrow_pollable_t pollables_ptr[2];
      pollables.ptr = pollables_ptr;
      pollables.ptr[0] = pollable_borrow;
      pollables.ptr[1] = poll_borrow_pollable(timeout_pollable);
      pollables.len = 2;
      wasip2_list_u32_t ret;
      poll_poll(&pollables, &ret);
      poll_pollable_drop_own(timeout_pollable);
      for (size_t i = 0; i < ret.len; i++) {
        if (ret.ptr[i] == 1) {
          // Timed out
          errno = EWOULDBLOCK;
          wasip2_list_u32_free(&ret);
          return -1;
        }
      }
      wasip2_list_u32_free(&ret);
    } else {
      poll_method_pollable_block(pollable_borrow);
    }
  }
}

ssize_t __wasilibc_read(wasip2_read_t *read, void *buffer, size_t length) {
  while (true) {
    wasip2_list_u8_t result;
    streams_stream_error_t error;

    // Attempt a read for the `length` we were passed in.
    if (!streams_method_input_stream_read(read->input, length, &result, &error))
      return wasip2_handle_read_error(error);

    // For empty reads after performing a successful 0-length read go ahead and
    // bail out.
    if (length == 0)
      return 0;

    // If bytes were read then copy those to the output `buffer`, deallocate
    // the list that the canonical ABI allocated, and return.
    if (result.len) {
      size_t len = result.len;
      memcpy(buffer, result.ptr, len);
      wasip2_list_u8_free(&result);
      if (read->offset)
        *read->offset += len;
      return len;
    }

    // Nonblocking reads bail out here as all that's left to do is block.
    if (!read->blocking) {
      errno = EWOULDBLOCK;
      return -1;
    }

    // Lazily initialize the pollable for this input stream.
    if (read->pollable->__handle == 0)
      *read->pollable = streams_method_input_stream_subscribe(read->input);
    poll_borrow_pollable_t pollable_borrow =
        poll_borrow_pollable(*read->pollable);

    // Wait either with a timeout or indefinitely for this read to complete.
    if (read->timeout != 0) {
      poll_own_pollable_t timeout_pollable =
          monotonic_clock_subscribe_duration(read->timeout);
      poll_list_borrow_pollable_t pollables;
      poll_borrow_pollable_t pollables_ptr[2];
      pollables.ptr = pollables_ptr;
      pollables.ptr[0] = pollable_borrow;
      pollables.ptr[1] = poll_borrow_pollable(timeout_pollable);
      pollables.len = 2;
      wasip2_list_u32_t ret;
      poll_poll(&pollables, &ret);
      poll_pollable_drop_own(timeout_pollable);
      for (size_t i = 0; i < ret.len; i++) {
        if (ret.ptr[i] == 1) {
          // Timed out
          errno = EWOULDBLOCK;
          wasip2_list_u32_free(&ret);
          return -1;
        }
      }
      wasip2_list_u32_free(&ret);
    } else {
      poll_method_pollable_block(pollable_borrow);
    }
  }
}

#endif // __wasip2__
