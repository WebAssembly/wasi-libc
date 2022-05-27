#include <common/net.h>

#include <sys/socket.h>

#include <assert.h>
#include <wasi/api.h>
#include <errno.h>
#include <string.h>

#define MIN(a,b) ((a)<(b) ? (a) : (b))

ssize_t recvfrom(int socket, void* buffer, size_t length, int flags, struct sockaddr *restrict addr, socklen_t *restrict addrlen) {
  // Prepare input parameters.
  __wasi_iovec_t iov = {.buf = buffer, .buf_len = length};
  __wasi_iovec_t *ri_data = &iov;
  size_t ri_data_len = 1;
  __wasi_riflags_t ri_flags = 0;

  if ((flags & MSG_PEEK) != 0) { ri_flags |= __WASI_RIFLAGS_RECV_PEEK; }
  if ((flags & MSG_WAITALL) != 0) { ri_flags |= __WASI_RIFLAGS_RECV_WAITALL; }
  if ((flags & MSG_TRUNC) != 0) { ri_flags |= __WASI_RIFLAGS_RECV_DATA_TRUNCATED; }

  // Perform system call.
  __wasi_size_t ro_datalen;
  __wasi_roflags_t ro_flags;
  __wasi_addr_port_t peer_addr;
  __wasi_errno_t error = __wasi_sock_recv_from(socket,
                                          ri_data, ri_data_len, ri_flags,
                                          &ro_datalen,
                                          &ro_flags,
										  &peer_addr);
  if (error != 0) {
    errno = errno_fixup_socket(socket, error);
    return -1;
  }

  wasi_to_sockaddr(&peer_addr, addr, addrlen);
  return ro_datalen;
}
