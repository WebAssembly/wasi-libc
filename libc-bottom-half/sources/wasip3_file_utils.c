#include <wasi/version.h>

#ifdef __wasip3__
#include <wasi/file_utils.h>

int __wasilibc_write_stream3(int fildes, wasip3_write_t **write_end,
                             off_t **off) {
  descriptor_table_entry_t *entry = descriptor_table_get_ref(fildes);
  if (!entry)
    return -1;
  if (!entry->vtable->get_write_stream3) {
    errno = EOPNOTSUPP;
    return -1;
  }
  return (*entry->vtable->get_write_stream3)(entry->data, write_end, off);
}

int __wasilibc_read_stream3(
    int fildes,
    filesystem_tuple2_stream_u8_future_result_void_error_code_t **stream,
    off_t **off) {
  descriptor_table_entry_t *entry = descriptor_table_get_ref(fildes);
  if (!entry)
    return -1;
  if (!entry->vtable->get_read_stream3) {
    errno = EOPNOTSUPP;
    return -1;
  }
  return (*entry->vtable->get_read_stream3)(entry->data, stream, off);
}
#endif // __wasip3__
