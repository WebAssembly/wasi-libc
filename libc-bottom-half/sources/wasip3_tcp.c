#include <wasi/api.h>

#ifdef __wasip3__

#include <errno.h>
#include <stdlib.h>
#include <wasi/descriptor_table.h>
#include <wasi/tcp.h>

static void tcp_free(void *data) {
  tcp_socket_t *tcp = (tcp_socket_t *)data;

  sockets_tcp_socket_drop_own(tcp->socket);

  free(tcp);
}

static descriptor_vtable_t tcp_vtable = {
    .free = tcp_free,
    .getsockopt = __wasilibc_tcp_getsockopt,
    .setsockopt = __wasilibc_tcp_setsockopt,
};

static int tcp_add(sockets_own_tcp_socket_t socket,
                   sockets_ip_address_family_t family, bool blocking,
                   tcp_socket_t **out) {
  tcp_socket_t *tcp = calloc(1, sizeof(tcp_socket_t));
  if (!tcp) {
    sockets_tcp_socket_drop_own(socket);
    errno = ENOMEM;
    return -1;
  }
  tcp->state.tag = TCP_SOCKET_STATE_UNBOUND;
  tcp->socket = socket;
  tcp->family = family;
  tcp->blocking = blocking;

  descriptor_table_entry_t entry;
  entry.vtable = &tcp_vtable;
  entry.data = tcp;
  if (out)
    *out = tcp;
  return descriptor_table_insert(entry);
}

int __wasilibc_add_tcp_socket(sockets_own_tcp_socket_t socket,
                              sockets_ip_address_family_t family,
                              bool blocking) {
  return tcp_add(socket, family, blocking, NULL);
}

#endif // __wasip3__
