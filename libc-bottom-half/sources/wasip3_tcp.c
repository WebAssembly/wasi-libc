#include <wasi/api.h>

#ifdef __wasip3__

#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <wasi/descriptor_table.h>
#include <wasi/tcp.h>

static descriptor_vtable_t tcp_vtable;

static void tcp_free(void *data) {
  tcp_socket_t *tcp = (tcp_socket_t *)data;

  sockets_tcp_socket_drop_own(tcp->socket);

  free(tcp);
}

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

static int tcp_set_blocking(void *data, bool blocking) {
  tcp_socket_t *tcp = (tcp_socket_t *)data;
  tcp->blocking = blocking;
  return 0;
}

static int tcp_fcntl_getfl(void *data) {
  tcp_socket_t *socket = (tcp_socket_t *)data;
  int flags = 0;
  if (!socket->blocking) {
    flags |= O_NONBLOCK;
  }
  return flags;
}

static int tcp_fcntl_setfl(void *data, int flags) {
  tcp_socket_t *socket = (tcp_socket_t *)data;
  if (flags & O_NONBLOCK) {
    socket->blocking = false;
  } else {
    socket->blocking = true;
  }
  return 0;
}

static descriptor_vtable_t tcp_vtable = {
    .free = tcp_free,

    .set_blocking = tcp_set_blocking,

    .getsockopt = __wasilibc_tcp_getsockopt,
    .setsockopt = __wasilibc_tcp_setsockopt,

    .fcntl_getfl = tcp_fcntl_getfl,
    .fcntl_setfl = tcp_fcntl_setfl,
};

#endif // __wasip3__
