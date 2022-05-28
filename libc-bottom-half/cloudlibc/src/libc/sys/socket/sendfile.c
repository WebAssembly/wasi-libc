#include <common/errno.h>
#include <common/net.h>

#include <sys/socket.h>

#include <assert.h>
#include <wasi/api.h>
#include <errno.h>
#include <string.h>

ssize_t sendfile(int socket, int in_fd, off_t *__ofs, size_t __count) {
  __wasi_errno_t error;
  uint64_t count = (uint64_t)__count;
  uint64_t ofs;
  if (__ofs != NULL) {
    ofs = (uint64_t)*__ofs;
  } else {
    error = __wasi_fd_tell(in_fd, &ofs);
    if (error != 0) {
      errno = error;
      return -1;
    }
  }

  // Perform system call.
  uint64_t so_datalen = 0;
  error = __wasi_sock_send_file(socket, in_fd, ofs, count, &so_datalen);
  if (error != 0) {
    errno = errno_fixup_socket(socket, error);
    return -1;
  }
  return (ssize_t)so_datalen;
}
