#include <errno.h>
#include <unistd.h>
#include <wasi/api.h>
#include <wasi/libc.h>

#ifdef __wasip2__
#include <wasi/descriptor_table.h>
#include <wasi/file_utils.h>
#endif

int __wasilibc_fd_renumber(int fd, int newfd) {
  // Scan the preopen fds before making any changes.
  __wasilibc_populate_preopens();

#ifdef __wasip2__
  if (descriptor_table_renumber(fd, newfd) < 0)
    return -1;
#else
  __wasi_errno_t error = __wasi_fd_renumber(fd, newfd);
  if (error != 0) {
    errno = error;
    return -1;
  }
#endif
  return 0;
}

int close(int fd) {
  // Scan the preopen fds before making any changes.
  __wasilibc_populate_preopens();

#ifdef __wasip2__
  if (descriptor_table_remove(fd) < 0)
    return -1;

#else
  __wasi_errno_t error = __wasi_fd_close(fd);
  if (error != 0) {
    errno = error;
    return -1;
  }
#endif // __wasip2__

  return 0;
}

weak void __wasilibc_populate_preopens(void) {
  // This version does nothing. It may be overridden by a version which does
  // something if `__wasilibc_find_abspath` or `__wasilibc_find_relpath` are
  // used.
}
