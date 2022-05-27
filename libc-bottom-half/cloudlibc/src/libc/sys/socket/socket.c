#include <sys/socket.h>

#include <assert.h>
#include <wasi/api.h>
#include <errno.h>
#include <string.h>

int socket(int domain, int ty, int protocol) {
  int fd;
  __wasi_errno_t error = __wasi_sock_open(domain, ty, protocol, &fd);
  if (error != 0) {
    errno = error;
    return -1;
  }

  return fd;
}
