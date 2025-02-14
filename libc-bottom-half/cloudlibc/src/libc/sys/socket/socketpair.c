#include <sys/socket.h>
#include <__header_netinet_in.h>

#include <assert.h>
#include <wasi/api.h>
#include <errno.h>
#include <string.h>

int socketpair(int domain, int ty, int protocol, int *restrict socket_vector) {
  int fd1, fd2;
  if(!protocol) {
    switch (ty)
    {
    case SOCK_STREAM:
      protocol = IPPROTO_TCP;
      break;
    case SOCK_DGRAM:
      protocol = IPPROTO_UDP;
      break;
    }
  }
  __wasi_errno_t error = __wasi_sock_pair(domain, ty, protocol, &fd1, &fd2);
  if (error != 0) {
    errno = error;
    return -1;
  }

  socket_vector[0] = fd1;
  socket_vector[1] = fd2;
  return 0;
}
