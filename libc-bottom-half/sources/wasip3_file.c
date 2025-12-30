#include <errno.h>
#include <wasi/api.h>

#ifdef __wasip3__
#include <assert.h>
#include <wasi/descriptor_table.h>

typedef struct {
  filesystem_own_descriptor_t file_handle;
  off_t offset;
  int oflag;
} file3_t;

static void file_close_streams(void *data) {
  file3_t *file = (file3_t *)data;
  abort();
}

static void file_free(void *data) {
  file3_t *file = (file3_t *)data;
  file_close_streams(data);
  filesystem_descriptor_drop_own(file->file_handle);
  free(file);
}

static int file_get_file(void *data, filesystem_borrow_descriptor_t *out_file) {
  file3_t *file = (file3_t *)data;
  *out_file = filesystem_borrow_descriptor(file->file_handle);
  return 0;
}

static int file_fstat(void *data, struct stat *buf) { abort(); }

static int file_seek_end(file3_t *file) { abort(); }

static off_t file_seek(void *data, off_t offset, int whence) { abort(); }

static int file_set_blocking(void *data, bool blocking) { abort(); }

static int file_fcntl_getfl(void *data) { abort(); }

static int file_fcntl_setfl(void *data, int flags) { abort(); }

static int file_read_stream(void *data, filesystem_stream_u8_t *out,
                            off_t **offs) {
  file3_t *file = (file3_t *)data;
  filesystem_tuple2_stream_u8_future_result_void_error_code_t res;
  filesystem_method_descriptor_read_via_stream(
      filesystem_borrow_descriptor(file->file_handle), file->offset, &res);
  *out = res.f0;
  filesystem_future_result_void_error_code_drop_readable(res.f1);
  return 0;
}

static descriptor_vtable_t file_vtable = {
    .free = file_free,
    .get_file = file_get_file,
    .set_blocking = file_set_blocking,
    .fstat = file_fstat,
    .seek = file_seek,
    .close_streams = file_close_streams,
    .fcntl_getfl = file_fcntl_getfl,
    .fcntl_setfl = file_fcntl_setfl,
    .get_read_stream3 = file_read_stream,
};

int __wasilibc_add_file(filesystem_own_descriptor_t file_handle, int oflag) {
  file3_t *file = calloc(1, sizeof(file3_t));
  if (!file) {
    filesystem_descriptor_drop_own(file_handle);
    errno = ENOMEM;
    return -1;
  }
  assert(file_handle.__handle != 0);
  file->file_handle = file_handle;
  file->oflag = oflag;

  descriptor_table_entry_t entry;
  entry.vtable = &file_vtable;
  entry.data = file;
  return descriptor_table_insert(entry);
}

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

// Gets an `input-stream` borrow from the `fd` provided.
int __wasilibc_read_stream3(int fd, filesystem_stream_u8_t *out, off_t **off) {
  descriptor_table_entry_t *entry = descriptor_table_get_ref(fd);
  if (!entry)
    return -1;
  if (!entry->vtable->get_read_stream3) {
    errno = EOPNOTSUPP;
    return -1;
  }
  if (entry->vtable->get_read_stream3(entry->data, out, off) < 0)
    return -1;
  assert(out != 0);
  return 0;
}

int __wasilibc_write_stream3(int fd, filesystem_stream_u8_t *out, off_t **off) {
  descriptor_table_entry_t *entry = descriptor_table_get_ref(fd);
  if (!entry)
    return -1;
  if (!entry->vtable->get_write_stream3) {
    errno = EOPNOTSUPP;
    return -1;
  }
  if (entry->vtable->get_write_stream3(entry->data, out, off) < 0)
    return -1;
  assert(out != 0);
  return 0;
}

#endif // __wasip3__
