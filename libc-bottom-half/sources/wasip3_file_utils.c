#include <wasi/version.h>

#ifdef __wasip3__
#include <assert.h>
#include <common/errors.h>
#include <errno.h>
#include <stddef.h>
#include <stdlib.h>
#include <wasi/file_utils.h>
#include <wasi/wasip3_block.h>

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

int wasip3_string_from_c(const char *s, wasip3_string_t *out) {
  int len = validate_utf8(s);
  if (len < 0) {
    errno = EILSEQ;
    return -1;
  }
  out->ptr = (uint8_t *)s;
  out->len = len;
  return 0;
}

ssize_t __wasilibc_write3(int fildes, void const *buf, size_t nbyte) {
  descriptor_table_entry_t *entry = descriptor_table_get_ref(fildes);
  if (!entry)
    return -1;
  if (!entry->vtable->get_write_stream3) {
    errno = EOPNOTSUPP;
    return -1;
  }
  off_t *off;
  wasip3_write_t *write_end;
  if ((*entry->vtable->get_write_stream3)(entry->data, &write_end, &off) < 0)
    return -1;
  wasip3_waitable_status_t status =
      filesystem_stream_u8_write(write_end->output, buf, nbyte);
  ssize_t amount = wasip3_waitable_block_on(status, write_end->output);
  if (amount > 0) {
    if (off)
      *off += amount;
    return amount;
  } else {
    wasip3_subtask_block_on(write_end->subtask);
    if (write_end->pending_result.is_err) {
      translate_error(write_end->pending_result.val.err);
      return -1;
    }
    // EOF
    return 0;
  }
}

ssize_t __wasilibc_read3(int fildes, void *buf, size_t nbyte) {
  descriptor_table_entry_t *entry = descriptor_table_get_ref(fildes);
  if (!entry)
    return -1;
  if (!entry->vtable->get_read_stream3) {
    errno = EOPNOTSUPP;
    return -1;
  }
  off_t *off;
  filesystem_tuple2_stream_u8_future_result_void_error_code_t *stream;
  if ((*entry->vtable->get_read_stream3)(entry->data, &stream, &off) < 0)
    return -1;
  wasip3_waitable_status_t status =
      filesystem_stream_u8_read(stream->f0, buf, nbyte);
  ssize_t amount = wasip3_waitable_block_on(status, stream->f0);
  if (amount > 0) {
    if (off)
      *off += amount;
    return amount;
  } else {
    filesystem_result_void_error_code_t error;
    status = filesystem_future_result_void_error_code_read(stream->f1, &error);
    amount = wasip3_waitable_block_on(status, stream->f1);
    if (amount > 0 && error.is_err) {
      translate_error(error.val.err);
      return -1;
    }
    // EOF
    return 0;
  }
}
#endif // __wasip3__
