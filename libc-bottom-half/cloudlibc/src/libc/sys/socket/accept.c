// SPDX-License-Identifier: BSD-2-Clause

#include <common/net.h>

#include <sys/socket.h>

#include <assert.h>
#include <wasi/api.h>
#include <errno.h>
#include <string.h>

int accept(int socket, struct sockaddr *restrict addr, socklen_t *restrict addrlen) {
  return accept4(socket, addr, addrlen, 0);
}

int accept4(int socket, struct sockaddr *restrict addr, socklen_t *restrict addrlen, int flags) {
  int ret = -1;
  
  if (flags & ~(SOCK_NONBLOCK | SOCK_CLOEXEC)) {
    errno = EINVAL;
    return -1;
  }

  __wasi_addr_port_t peer_addr;
  __wasi_errno_t error = __wasi_sock_accept(socket, (flags & SOCK_NONBLOCK) ? __WASI_FDFLAGS_NONBLOCK : 0, &ret, &peer_addr);

  if (error != 0) {
    errno = error;
    return -1;
  }

  wasi_to_sockaddr(&peer_addr, addr, addrlen);  
  return ret;
}
