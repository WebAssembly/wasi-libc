#ifdef __wasilibc_use_wasip2
#include <common/errors.h>
#include <unistd.h>
#include <wasi/descriptor_table.h>
#include <wasi/wasip2.h>
#else
#include <wasi/api.h>
#endif
#include <errno.h>

off_t __wasilibc_tell(int fildes) {
#ifdef __wasilibc_use_wasip2
  // Look up a stream for fildes
  descriptor_table_entry_t *entry = descriptor_table_get_ref(fildes);
  if (!entry)
    return -1;
  if (!entry->vtable->seek) {
    errno = EINVAL;
    return -1;
  }
  return entry->vtable->seek(entry->data, 0, SEEK_CUR);
#else
  __wasi_filesize_t offset;
  __wasi_errno_t error = __wasi_fd_tell(fildes, &offset);
  if (error != 0) {
    // lseek returns ESPIPE on when called on a pipe, socket, or fifo,
    // which on WASI would translate into ENOTCAPABLE.
    errno = error == ENOTCAPABLE ? ESPIPE : error;
    return -1;
  }
  return offset;
#endif
}
