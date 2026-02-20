#include <wasi/api.h>

#ifdef __wasip3__

#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <wasi/descriptor_table.h>
#include <wasi/sockets_utils.h>
#include <wasi/udp.h>
#include <wasi/wasip3.h>

typedef struct {
  int dummy;
} udp_socket_state_unbound_t;
typedef struct {
  int dummy;
} udp_socket_state_bound_t;

typedef struct {
  enum {
    UDP_SOCKET_STATE_UNBOUND,
    UDP_SOCKET_STATE_BOUND,
  } tag;
  union {
    udp_socket_state_unbound_t unbound;
    udp_socket_state_bound_t bound;
  };
} udp_socket_state_t;

typedef struct {
  sockets_own_udp_socket_t socket;
  bool blocking;
  sockets_ip_address_family_t family;
  udp_socket_state_t state;
} udp_socket_t;

static descriptor_vtable_t udp_vtable;

int __wasilibc_add_udp_socket(sockets_own_udp_socket_t socket,
                              sockets_ip_address_family_t family,
                              bool blocking) {
  udp_socket_t *udp = calloc(1, sizeof(udp_socket_t));
  if (!udp) {
    sockets_udp_socket_drop_own(socket);
    errno = ENOMEM;
    return -1;
  }
  udp->state.tag = UDP_SOCKET_STATE_UNBOUND;
  udp->socket = socket;
  udp->family = family;
  udp->blocking = blocking;

  descriptor_table_entry_t entry;
  entry.vtable = &udp_vtable;
  entry.data = udp;
  return descriptor_table_insert(entry);
}

static void udp_free(void *data) {
  udp_socket_t *udp = (udp_socket_t *)data;

  sockets_udp_socket_drop_own(udp->socket);

  free(udp);
}

static int udp_set_blocking(void *data, bool blocking) {
  udp_socket_t *udp = (udp_socket_t *)data;
  udp->blocking = blocking;
  return 0;
}

static int udp_fstat(void *data, struct stat *buf) {
  udp_socket_t *udp = (udp_socket_t *)data;
  memset(buf, 0, sizeof(struct stat));
  buf->st_mode = S_IFSOCK;
  return 0;
}

static int udp_bind(void *data, const struct sockaddr *addr,
                    socklen_t addrlen) {
  udp_socket_t *socket = (udp_socket_t *)data;
  sockets_ip_socket_address_t local_address;
  int parse_err;
  if (!__wasi_sockets_utils__parse_address(socket->family, addr, addrlen,
                                           &local_address, &parse_err)) {
    errno = parse_err;
    return -1;
  }
  if (socket->state.tag != UDP_SOCKET_STATE_UNBOUND) {
    errno = EINVAL;
    return -1;
  }

  sockets_error_code_t error;
  sockets_borrow_udp_socket_t socket_borrow =
      sockets_borrow_udp_socket(socket->socket);

  if (!sockets_method_udp_socket_bind(socket_borrow, &local_address, &error)) {
    errno = __wasi_sockets_utils__map_error(error);
    return -1;
  }

  // Bind successful.
  socket->state.tag = UDP_SOCKET_STATE_BOUND;
  return 0;
}

static int udp_fcntl_getfl(void *data) {
  udp_socket_t *socket = (udp_socket_t *)data;
  int flags = 0;
  if (!socket->blocking) {
    flags |= O_NONBLOCK;
  }
  return flags;
}

static int udp_fcntl_setfl(void *data, int flags) {
  udp_socket_t *socket = (udp_socket_t *)data;
  if (flags & O_NONBLOCK) {
    socket->blocking = false;
  } else {
    socket->blocking = true;
  }
  return 0;
}

static descriptor_vtable_t udp_vtable = {
    .free = udp_free,

    .set_blocking = udp_set_blocking,
    .fstat = udp_fstat,

    .bind = udp_bind,

    .fcntl_getfl = udp_fcntl_getfl,
    .fcntl_setfl = udp_fcntl_setfl,
};

#endif // __wasip3__
