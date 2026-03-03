#include <errno.h>
#include <sys/types.h>
#include <wasi/api.h>

#ifndef __wasip1__
#include <common/errors.h>
#include <unistd.h>
#include <wasi/descriptor_table.h>
#endif

off_t __wasilibc_tell(int fildes) {
#if defined(__wasip1__)
  __wasi_filesize_t offset;
  __wasi_errno_t error = __wasi_fd_tell(fildes, &offset);
  if (error != 0) {
    // lseek returns ESPIPE on when called on a pipe, socket, or fifo,
    // which on WASI would translate into ENOTCAPABLE.
    errno = error == ENOTCAPABLE ? ESPIPE : error;
    return -1;
  }
  return offset;
#elif defined(__wasip2__) || defined(__wasip3__)
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
#error "Unsupported WASI version"
#endif
}
