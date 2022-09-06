#include <sys/socket.h>

#include <assert.h>
#include <wasi/api.h>
#include <errno.h>
#include <string.h>

int socketpair(int domain, int ty, int protocol, int *restrict socket_vector) {
  int fd1, fd2;
  __wasi_errno_t error = __wasi_fd_pipe(&fd1, &fd2);
  if (error != 0) {
    errno = error;
    return -1;
  }

  socket_vector[0] = fd1;
  socket_vector[1] = fd2;
  return 0;
}
