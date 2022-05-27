#include <common/errno.h>
#include <common/net.h>
#include <sys/socket.h>

#include <assert.h>
#include <wasi/api.h>
#include <errno.h>
#include <string.h>

int getpeername(int socket, struct sockaddr *restrict addr, socklen_t *restrict addrlen) {
  __wasi_addr_port_t peer_addr;
  __wasi_errno_t error = __wasi_sock_addr_peer(socket, &peer_addr);
  if (error != 0) {
    errno = errno_fixup_socket(socket, error);
    return -1;
  }

  wasi_to_sockaddr(&peer_addr, addr, addrlen);
  return 0;
}
