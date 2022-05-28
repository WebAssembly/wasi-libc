#include <common/errno.h>
#include <common/net.h>

#include <sys/socket.h>

#include <assert.h>
#include <wasi/api.h>
#include <errno.h>
#include <string.h>

ssize_t sendto(int socket, const void *restrict buffer, size_t length, int flags, const struct sockaddr *restrict addr, socklen_t addrlen) {
  // This implementation does not support any flags.
  if (flags != 0) {
    errno = EOPNOTSUPP;
    return -1;
  }

  if (buffer == NULL || addr == NULL) {
	  errno = EINVAL;
	  return -1;
  }

  // Prepare input parameters.
  __wasi_ciovec_t iov = {.buf = buffer, .buf_len = length};
  __wasi_ciovec_t *si_data = &iov;
  size_t si_data_len = 1;
  __wasi_siflags_t si_flags = 0;

  __wasi_addr_port_t peer_addr;
  __wasi_errno_t error = sockaddr_to_wasi(addr, addrlen, &peer_addr);
  if (error != 0) {
    errno = error;
    return -1;
  }

  // Perform system call.
  __wasi_size_t so_datalen;
  error = __wasi_sock_send_to(socket, si_data, si_data_len, si_flags, &peer_addr, &so_datalen);
  if (error != 0) {
    errno = errno_fixup_socket(socket, error);
    return -1;
  }
  return so_datalen;
}
