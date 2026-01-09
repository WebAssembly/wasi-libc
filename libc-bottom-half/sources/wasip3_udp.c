#include <errno.h>
#include <wasi/api.h>

#ifdef __wasip3__

int __wasilibc_add_udp_socket(sockets_own_tcp_socket_t socket,
                              sockets_ip_address_family_t family,
                              bool blocking) {
  // TODO(wasip3)
  errno = ENOTSUP;
  return -1;
}
#endif // __wasip3__
