#include <errno.h>
#include <wasi/api.h>

#ifdef __wasip3__
#include <wasi/descriptor_table.h>
#include <assert.h>

int __wasilibc_add_file(filesystem_own_descriptor_t file_handle, int oflag) {
  // TODO(wasip3)
  errno = EOPNOTSUPP;
  return -1;
}

int __wasilibc_write_stream3(int fd,
                            filesystem_stream_u8_t *out,
                            off_t **off) {
  descriptor_table_entry_t* entry = descriptor_table_get_ref(fd);
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
