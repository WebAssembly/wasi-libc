#include <errno.h>
#include <wasi/api.h>

#ifdef __wasip3__
#include <netinet/tcp.h>
#include <stdlib.h>
#include <wasi/descriptor_table.h>
#include <wasi/file_utils.h>
#include <wasi/sockets_utils.h>

typedef struct {
  sockets_own_tcp_socket_t socket;
  sockets_ip_address_family_t family;
  bool blocking;
  sockets_stream_own_tcp_socket_t incoming;
} tcp_socket_t;

static int tcp_connect(void *data, const struct sockaddr *addr,
                       socklen_t addrlen) {
  tcp_socket_t *socket = (tcp_socket_t *)data;
  sockets_method_tcp_socket_connect_args_t args;
  // sockets_ip_socket_address_t remote_address;
  int parse_err;
  if (!__wasi_sockets_utils__parse_address(socket->family, addr, addrlen,
                                           &args.remote_address, &parse_err)) {
    errno = parse_err;
    return -1;
  }

  args.self = sockets_borrow_tcp_socket(socket->socket);
  sockets_result_void_error_code_t error;

  wasip3_subtask_status_t status =
      sockets_method_tcp_socket_connect(&args, &error);
  wasip3_subtask_block_on(status);

  if (error.is_err) {
    errno = __wasi_sockets_utils__map_error(error.val.err);
    return -1;
  }

  return 0;
}

static int tcp_do_bind(tcp_socket_t *socket,
                       sockets_ip_socket_address_t *address) {
  sockets_error_code_t error;
  sockets_borrow_tcp_socket_t socket_borrow =
      sockets_borrow_tcp_socket(socket->socket);

  if (!sockets_method_tcp_socket_bind(socket_borrow, address, &error)) {
    errno = __wasi_sockets_utils__map_error(error);
    return -1;
  }
  return 0;
}

static int tcp_bind(void *data, const struct sockaddr *addr,
                    socklen_t addrlen) {
  tcp_socket_t *socket = (tcp_socket_t *)data;
  sockets_ip_socket_address_t local_address;
  int parse_err;
  if (!__wasi_sockets_utils__parse_address(socket->family, addr, addrlen,
                                           &local_address, &parse_err)) {
    errno = parse_err;
    return -1;
  }
  return tcp_do_bind(socket, &local_address);
}

static int tcp_getsockname(void *data, struct sockaddr *addr,
                           socklen_t *addrlen) {
  tcp_socket_t *socket = (tcp_socket_t *)data;
  output_sockaddr_t output_addr;
  if (!__wasi_sockets_utils__output_addr_validate(socket->family, addr, addrlen,
                                                  &output_addr)) {
    errno = EINVAL;
    return -1;
  }

  if (output_addr.tag == OUTPUT_SOCKADDR_NULL) {
    errno = EINVAL;
    return -1;
  }

  sockets_error_code_t error;
  sockets_ip_socket_address_t result;
  sockets_borrow_tcp_socket_t socket_borrow =
      sockets_borrow_tcp_socket(socket->socket);
  if (!sockets_method_tcp_socket_get_local_address(socket_borrow, &result,
                                                   &error)) {
    errno = __wasi_sockets_utils__map_error(error);
    return -1;
  }

  __wasi_sockets_utils__output_addr_write(result, &output_addr);
  return 0;
}

static int tcp_listen(void *data, int backlog) {
  tcp_socket_t *socket = (tcp_socket_t *)data;
  sockets_error_code_t error;
  sockets_borrow_tcp_socket_t socket_borrow =
      sockets_borrow_tcp_socket(socket->socket);

  if (!sockets_method_tcp_socket_set_listen_backlog_size(socket_borrow, backlog,
                                                         &error)) {
    errno = __wasi_sockets_utils__map_error(error);
    return -1;
  }

  // sockets_stream_own_tcp_socket_t ret;
  if (!sockets_method_tcp_socket_listen(socket_borrow, &socket->incoming,
                                        &error)) {
    errno = __wasi_sockets_utils__map_error(error);
    return -1;
  }

  return 0;
}

int tcp_setsockopt(void *data, int level, int optname, const void *optval,
                   socklen_t optlen) {
  tcp_socket_t *socket = (tcp_socket_t *)data;
  int intval = *(int *)optval;

  switch (level) {
  case SOL_TCP:
    switch (optname) {
    case TCP_NODELAY: {
      // is this right?
      socket->blocking = false;
      return 0;
    }
    default:
      errno = ENOPROTOOPT;
      return -1;
    }
    break;

  default:
    errno = ENOPROTOOPT;
    return -1;
  }
}

static descriptor_vtable_t tcp_vtable = {
    .bind = tcp_bind,
    .connect = tcp_connect,
    .getsockname = tcp_getsockname,
    .listen = tcp_listen,
    // .getpeername = tcp_getpeername,
    // .recvfrom = tcp_recvfrom,
    // .sendto = tcp_sendto,
    // .shutdown = tcp_shutdown,
    // .getsockopt = tcp_getsockopt,
    .setsockopt = tcp_setsockopt,
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
  tcp->incoming = 0;

  descriptor_table_entry_t entry;
  entry.vtable = &tcp_vtable;
  entry.data = tcp;
  return descriptor_table_insert(entry);
}
#endif // __wasip3__
