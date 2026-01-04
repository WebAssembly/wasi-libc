#include <wasi/file_utils.h>

#ifdef __wasip3__

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

int __wasilibc_read3(int fd, void *buf, size_t nbyte, waitable_t *waitable,
                     wasip3_waitable_status_t *out, off_t **off) {
  descriptor_table_entry_t *entry = descriptor_table_get_ref(fd);
  if (!entry)
    return -1;
  if (!entry->vtable->read3) {
    errno = EOPNOTSUPP;
    return -1;
  }
  if (entry->vtable->read3(entry->data, buf, nbyte, waitable, out, off) < 0)
    return -1;
  return 0;
}

int __wasilibc_write3(int fd, void const *buf, size_t nbyte,
                      waitable_t *waitable, wasip3_waitable_status_t *out,
                      off_t **off) {
  descriptor_table_entry_t *entry = descriptor_table_get_ref(fd);
  if (!entry)
    return -1;
  if (!entry->vtable->write3) {
    errno = EOPNOTSUPP;
    return -1;
  }
  if (entry->vtable->write3(entry->data, buf, nbyte, waitable, out, off) < 0)
    return -1;
  return 0;
}

void wasip3_subtask_block_on(wasip3_subtask_status_t status) {
  if (status != WASIP3_SUBTASK_RETURNED) {
    if (WASIP3_SUBTASK_STATE(status) == WASIP3_SUBTASK_STARTED) {
      wasip3_subtask_t handle = WASIP3_SUBTASK_HANDLE(status);
      wasip3_waitable_set_t set = wasip3_waitable_set_new();
      wasip3_waitable_join(handle, set);
      wasip3_event_t event;
      wasip3_waitable_set_wait(set, &event);
      assert(event.event == WASIP3_EVENT_SUBTASK);
      assert(event.code == WASIP3_SUBTASK_RETURNED);
      wasip3_subtask_drop(event.waitable);
      wasip3_waitable_set_drop(set);
    } else {
      abort();
    }
  }
}

#endif
