#include <errno.h>
#include <common/net.h>
#include <sys/socket.h>

#include <assert.h>
#include <wasi/api.h>
#include <errno.h>
#include <string.h>

int getsockname(int socket, struct sockaddr *restrict addr, socklen_t *restrict addrlen) {
  __wasi_addr_port_t local_addr;
  __wasi_errno_t error = __wasi_sock_addr_local(socket, &local_addr);
  if (error == ENOTSOCK) {
    // in that case, return a AF_LOCAL type constant
    *addrlen = sizeof(addr->sa_family);
    addr->sa_family = AF_LOCAL;
    return 0;
  }
  if (error != 0) {
    errno = error;
    return -1;
  }

  wasi_to_sockaddr(&local_addr, addr, addrlen);
  return 0;
}
