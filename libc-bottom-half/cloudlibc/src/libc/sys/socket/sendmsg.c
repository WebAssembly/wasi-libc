#include <common/errno.h>
#include <common/net.h>

#include <sys/socket.h>
#include <__struct_msghdr.h>

#include <assert.h>
#include <wasi/api.h>
#include <errno.h>
#include <string.h>

ssize_t sendmsg(int socket, const struct msghdr* msg, int flags) {
  // This implementation does not support any flags.
  if (flags != 0) {
    errno = EOPNOTSUPP;
    return -1;
  }

  if (msg->msg_iov == NULL) {
	errno = EINVAL;
	return -1;
  }

  __wasi_ciovec_t *si_data = (__wasi_ciovec_t *)msg->msg_iov;
  size_t si_data_len = msg->msg_iovlen;

  __wasi_size_t so_datalen;
  __wasi_siflags_t si_flags = 0;
  __wasi_errno_t error;
  if (msg->msg_name == NULL) {
	error = __wasi_sock_send(socket, si_data, si_data_len, si_flags, &so_datalen);
  } else {
    struct sockaddr *addr = (struct sockaddr *)msg->msg_name;
	socklen_t addrlen = msg->msg_namelen;
	__wasi_addr_port_t peer_addr;
    error = sockaddr_to_wasi(addr, addrlen, &peer_addr);
    if (error != 0) {
      errno = error;
      return -1;
    }
    error = __wasi_sock_send_to(socket, si_data, si_data_len, si_flags, &peer_addr, &so_datalen);
  }

  if (error != 0) {
    errno = errno_fixup_socket(socket, error);
    return -1;
  }
  return so_datalen;
}
