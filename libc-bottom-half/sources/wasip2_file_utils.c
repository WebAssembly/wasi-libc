#include <assert.h>
#include <common/errors.h>
#include <errno.h>
#include <stddef.h>
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

// Gets an `output-stream` borrow from the `fd` provided.
int __wasilibc_write_stream(int fd, streams_borrow_output_stream_t *out,
                            off_t **off, poll_borrow_pollable_t *pollable) {
  descriptor_table_entry_t *entry = descriptor_table_get_ref(fd);
  if (!entry)
    return -1;
  if (!entry->vtable->get_write_stream) {
    errno = EOPNOTSUPP;
    return -1;
  }
  poll_own_pollable_t *own_pollable;
  if (entry->vtable->get_write_stream(entry->data, out, off, &own_pollable) < 0)
    return -1;
  assert(out->__handle != 0);
  if (pollable) {
    if (own_pollable->__handle == 0) {
      *own_pollable = streams_method_output_stream_subscribe(*out);
    }
    *pollable = poll_borrow_pollable(*own_pollable);
  }
  return 0;
}

// Gets an `input-stream` borrow from the `fd` provided.
int __wasilibc_read_stream(int fd, streams_borrow_input_stream_t *out,
                           off_t **off, poll_borrow_pollable_t *pollable) {
  descriptor_table_entry_t *entry = descriptor_table_get_ref(fd);
  if (!entry)
    return -1;
  if (!entry->vtable->get_read_stream) {
    errno = EOPNOTSUPP;
    return -1;
  }
  poll_own_pollable_t *own_pollable;
  if (entry->vtable->get_read_stream(entry->data, out, off, &own_pollable) < 0)
    return -1;
  assert(out->__handle != 0);
  if (pollable) {
    if (own_pollable->__handle == 0) {
      *own_pollable = streams_method_input_stream_subscribe(*out);
    }
    *pollable = poll_borrow_pollable(*own_pollable);
  }
  return 0;
}

#endif // __wasip2__
