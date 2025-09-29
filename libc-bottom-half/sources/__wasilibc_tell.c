#ifdef __wasilibc_use_wasip2
#include <wasi/wasip2.h>
#include <wasi/descriptor_table.h>
#include <common/errors.h>
#else
#include <wasi/api.h>
#endif
#include <errno.h>

off_t __wasilibc_tell(int fildes) {
#ifdef __wasilibc_use_wasip2
  // Look up a stream for fildes
  descriptor_table_entry_t *entry;
  if (!descriptor_table_get_ref(fildes, &entry)) {
    errno = EBADF;
    return -1;
  }

  // Return the current offset in the stream
  if (entry->tag == DESCRIPTOR_TABLE_ENTRY_FILE_STREAM) {
    return entry->stream.offset;
  }
  errno = EINVAL;
  return -1;
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
