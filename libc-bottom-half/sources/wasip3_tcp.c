#include <errno.h>
#include <wasi/api.h>

#ifdef __wasip3__
#include <wasi/descriptor_table.h>
#include <stdlib.h>

typedef struct {
  sockets_own_tcp_socket_t socket;
  sockets_ip_address_family_t family;
  bool blocking;
} tcp_socket_t;

static int tcp_connect(void *data, const struct sockaddr *addr,
                       socklen_t addrlen) {
  abort();
}

static descriptor_vtable_t tcp_vtable = {
  .connect = tcp_connect,
  // .getsockname = tcp_getsockname,
  // .getpeername = tcp_getpeername,
  // .recvfrom = tcp_recvfrom,
  // .sendto = tcp_sendto,
  // .shutdown = tcp_shutdown,
  // .getsockopt = tcp_getsockopt,
  // .setsockopt = tcp_setsockopt,
};

int __wasilibc_add_tcp_socket(sockets_own_tcp_socket_t socket,
                              sockets_ip_address_family_t family,
                              bool blocking) {
  tcp_socket_t *tcp = calloc(1, sizeof(tcp_socket_t));
  if (!tcp) {
    sockets_tcp_socket_drop_own(socket);
    errno = ENOMEM;
    return -1;
  }
  tcp->socket = socket;
  tcp->family = family;
  tcp->blocking = blocking;

  descriptor_table_entry_t entry;
  entry.vtable = &tcp_vtable;
  entry.data = tcp;
  return descriptor_table_insert(entry);
}
#endif // __wasip3__
