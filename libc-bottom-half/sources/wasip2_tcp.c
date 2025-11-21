#ifdef __wasilibc_use_wasip2

#include "common/time.h"
#include <errno.h>
#include <limits.h>
#include <netinet/tcp.h>
#include <string.h>
#include <wasi/descriptor_table.h>
#include <wasi/file_utils.h>
#include <wasi/sockets_utils.h>
#include <wasi/tcp.h>
#include <wasi/wasip2.h>

const uint64_t NS_PER_S = 1000000000;

typedef struct {
  int dummy;
} tcp_socket_state_unbound_t;
typedef struct {
  int dummy;
} tcp_socket_state_bound_t;
typedef struct {
  int dummy;
} tcp_socket_state_connecting_t;
typedef struct {
  int dummy;
} tcp_socket_state_listening_t;

// Pollables here are lazily initialized on-demand.
typedef struct {
  streams_own_input_stream_t input;
  poll_own_pollable_t input_pollable;
  streams_own_output_stream_t output;
  poll_own_pollable_t output_pollable;
} tcp_socket_state_connected_t;

typedef struct {
  network_error_code_t error_code;
} tcp_socket_state_connect_failed_t;

// This is a tagged union. When adding/removing/renaming cases, be sure to keep the tag and union definitions in sync.
typedef struct {
  enum {
    TCP_SOCKET_STATE_UNBOUND,
    TCP_SOCKET_STATE_BOUND,
    TCP_SOCKET_STATE_CONNECTING,
    TCP_SOCKET_STATE_CONNECTED,
    TCP_SOCKET_STATE_CONNECT_FAILED,
    TCP_SOCKET_STATE_LISTENING,
  } tag;
  union {
    tcp_socket_state_unbound_t unbound;
    tcp_socket_state_bound_t bound;
    tcp_socket_state_connecting_t connecting;
    tcp_socket_state_connected_t connected;
    tcp_socket_state_connect_failed_t connect_failed;
    tcp_socket_state_listening_t listening;
  };
} tcp_socket_state_t;

typedef struct {
  tcp_own_tcp_socket_t socket;
  // This pollable is lazily initialized on-demand.
  poll_own_pollable_t socket_pollable;
  bool blocking;
  bool fake_nodelay;
  bool fake_reuseaddr;
  network_ip_address_family_t family;
  tcp_socket_state_t state;
  monotonic_clock_duration_t send_timeout;
  monotonic_clock_duration_t recv_timeout;
} tcp_socket_t;

static descriptor_vtable_t tcp_vtable;

static int tcp_add(tcp_own_tcp_socket_t socket,
                   network_ip_address_family_t family,
                   bool blocking,
                   tcp_socket_t **out) {
  tcp_socket_t *tcp = calloc(1, sizeof(tcp_socket_t));
  if (!tcp) {
    tcp_tcp_socket_drop_own(socket);
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

int __wasilibc_add_tcp_socket(tcp_own_tcp_socket_t socket,
                              network_ip_address_family_t family,
                              bool blocking) {
  return tcp_add(socket, family, blocking, NULL);
}

static void tcp_free(void *data) {
  tcp_socket_t *tcp = (tcp_socket_t *)data;

  switch (tcp->state.tag) {
    case TCP_SOCKET_STATE_CONNECTED:
      if (tcp->state.connected.input_pollable.__handle != 0)
        poll_pollable_drop_own(tcp->state.connected.input_pollable);
      if (tcp->state.connected.output_pollable.__handle != 0)
        poll_pollable_drop_own(tcp->state.connected.output_pollable);
      streams_input_stream_drop_own(tcp->state.connected.input);
      streams_output_stream_drop_own(tcp->state.connected.output);
      break;
    default:
      break;
  }

  if (tcp->socket_pollable.__handle != 0)
    poll_pollable_drop_own(tcp->socket_pollable);
  tcp_tcp_socket_drop_own(tcp->socket);

  free(tcp);
}

static int tcp_get_read_stream(void *data,
                               streams_borrow_input_stream_t *out_stream,
                               off_t **out_offset,
                               poll_own_pollable_t **out_pollable) {
  tcp_socket_t *tcp = (tcp_socket_t *)data;

  if (tcp->state.tag != TCP_SOCKET_STATE_CONNECTED) {
    errno = ENOTCONN;
    return -1;
  }
  *out_stream = streams_borrow_input_stream(tcp->state.connected.input);
  if (out_offset)
    *out_offset = NULL;
  if (out_pollable)
    *out_pollable = &tcp->state.connected.input_pollable;
  return 0;
}

static int tcp_get_write_stream(void *data,
                                streams_borrow_output_stream_t *out_stream,
                                off_t **out_offset,
                                poll_own_pollable_t **out_pollable) {
  tcp_socket_t *tcp = (tcp_socket_t *)data;

  if (tcp->state.tag != TCP_SOCKET_STATE_CONNECTED) {
    errno = ENOTCONN;
    return -1;
  }
  *out_stream = streams_borrow_output_stream(tcp->state.connected.output);
  if (out_offset)
    *out_offset = NULL;
  if (out_pollable)
    *out_pollable = &tcp->state.connected.output_pollable;
  return 0;
}

static int tcp_set_blocking(void *data, bool blocking) {
  tcp_socket_t *tcp = (tcp_socket_t *)data;
  tcp->blocking = blocking;
  return 0;
}

static int tcp_fstat(void *data, struct stat *buf) {
  tcp_socket_t *tcp = (tcp_socket_t *)data;
  memset(buf, 0, sizeof(struct stat));
  buf->st_mode = S_IFSOCK;
  return 0;
}

static poll_borrow_pollable_t tcp_pollable(tcp_socket_t *socket) {
  if (socket->socket_pollable.__handle == 0) {
    tcp_borrow_tcp_socket_t socket_borrow = tcp_borrow_tcp_socket(socket->socket);
    socket->socket_pollable = tcp_method_tcp_socket_subscribe(socket_borrow);
  }
  return poll_borrow_pollable(socket->socket_pollable);
}

static int tcp_handle_error(tcp_socket_t *socket,
                            network_error_code_t error) {
  if (error == NETWORK_ERROR_CODE_WOULD_BLOCK && socket->blocking) {
    poll_method_pollable_block(tcp_pollable(socket));
  } else {
    errno = __wasi_sockets_utils__map_error(error);
    return -1;
  }

  return 0;
}

static int tcp_accept4(void *data,
                       struct sockaddr *addr,
                       socklen_t *addrlen,
                       int flags) {
  tcp_socket_t *socket = (tcp_socket_t *)data;
  output_sockaddr_t output_addr;
  if (!__wasi_sockets_utils__output_addr_validate(socket->family, addr, addrlen, &output_addr)) {
    errno = EINVAL;
    return -1;
  }

  tcp_socket_state_listening_t listener;
  if (socket->state.tag == TCP_SOCKET_STATE_LISTENING) {
    listener = socket->state.listening;
  } else {
    errno = EINVAL;
    return -1;
  }

  tcp_borrow_tcp_socket_t socket_borrow = tcp_borrow_tcp_socket(socket->socket);

  tcp_tuple3_own_tcp_socket_own_input_stream_own_output_stream_t client_and_io;
  network_error_code_t error;
  while (!tcp_method_tcp_socket_accept(socket_borrow, &client_and_io, &error)) {
    if (tcp_handle_error(socket, error) < 0)
      return -1;
  }

  tcp_own_tcp_socket_t client = client_and_io.f0;
  tcp_borrow_tcp_socket_t client_borrow = tcp_borrow_tcp_socket(client);
  streams_own_input_stream_t input = client_and_io.f1;
  streams_own_output_stream_t output = client_and_io.f2;

  if (output_addr.tag != OUTPUT_SOCKADDR_NULL) {
    network_ip_socket_address_t remote_address;
    if (!tcp_method_tcp_socket_remote_address(client_borrow, &remote_address, &error)) {
      // TODO wasi-sockets: How to recover from this in a POSIX compatible way?
      abort();
    }

    __wasi_sockets_utils__output_addr_write(remote_address, &output_addr);
  }

  tcp_socket_t *client_socket;
  int client_fd = tcp_add(client, socket->family, (flags & SOCK_NONBLOCK) == 0, &client_socket);
  if (client_fd < 0) {
    streams_input_stream_drop_own(input);
    streams_output_stream_drop_own(output);
    return -1;
  }

  client_socket->state.tag = TCP_SOCKET_STATE_CONNECTED;
  memset(&client_socket->state.connected, 0, sizeof(client_socket->state.connected));
  client_socket->state.connected.input = input;
  client_socket->state.connected.output = output;
  client_socket->fake_nodelay = socket->fake_nodelay;
  return client_fd;
}

static int tcp_do_bind(tcp_socket_t *socket, network_ip_socket_address_t *address) {
  if (socket->state.tag != TCP_SOCKET_STATE_UNBOUND) {
    errno = EINVAL;
    return -1;
  }

  network_error_code_t error;
  network_borrow_network_t network_borrow = __wasi_sockets_utils__borrow_network();
  tcp_borrow_tcp_socket_t socket_borrow = tcp_borrow_tcp_socket(socket->socket);

  if (!tcp_method_tcp_socket_start_bind(socket_borrow, network_borrow, address, &error)) {
    errno = __wasi_sockets_utils__map_error(error);
    return -1;
  }

  // Bind has successfully started. Attempt to finish it:
  while (!tcp_method_tcp_socket_finish_bind(socket_borrow, &error)) {
    if (tcp_handle_error(socket, error) < 0)
      return -1;
  }

  // Bind successful.
  socket->state.tag = TCP_SOCKET_STATE_BOUND;
  return 0;
}

static int tcp_bind(void *data, const struct sockaddr *addr, socklen_t addrlen) {
  tcp_socket_t *socket = (tcp_socket_t *)data;
  network_ip_socket_address_t local_address;
  int parse_err;
  if (!__wasi_sockets_utils__parse_address(socket->family, addr, addrlen, &local_address, &parse_err)) {
    errno = parse_err;
    return -1;
  }
  return tcp_do_bind(socket, &local_address);
}

static int tcp_connect(void *data, const struct sockaddr *addr, socklen_t addrlen) {
  tcp_socket_t *socket = (tcp_socket_t *)data;
  network_ip_socket_address_t remote_address;
  int parse_err;
  if (!__wasi_sockets_utils__parse_address(socket->family, addr, addrlen,
	&remote_address, &parse_err)) {
    errno = parse_err;
    return -1;
  }

  switch (socket->state.tag) {
    case TCP_SOCKET_STATE_UNBOUND:
    case TCP_SOCKET_STATE_BOUND:
      // These can initiate a connect.
      break;
    case TCP_SOCKET_STATE_CONNECTING:
      errno = EALREADY;
      return -1;
    case TCP_SOCKET_STATE_CONNECTED:
      errno = EISCONN;
      return -1;
    case TCP_SOCKET_STATE_CONNECT_FAILED: // POSIX: "If connect() fails, the state of the socket is unspecified. Conforming applications should close the file descriptor and create a new socket before attempting to reconnect."
    case TCP_SOCKET_STATE_LISTENING:
    default:
      errno = EOPNOTSUPP;
      return -1;
  }

  network_error_code_t error;
  network_borrow_network_t network_borrow = __wasi_sockets_utils__borrow_network();
  tcp_borrow_tcp_socket_t socket_borrow = tcp_borrow_tcp_socket(socket->socket);

  if (!tcp_method_tcp_socket_start_connect(socket_borrow, network_borrow,
	&remote_address, &error)) {
    errno = __wasi_sockets_utils__map_error(error);
    return -1;
  }

  // Connect has successfully started.
  socket->state.tag = TCP_SOCKET_STATE_CONNECTING;

  // Attempt to finish it:
  tcp_tuple2_own_input_stream_own_output_stream_t io;
  while (!tcp_method_tcp_socket_finish_connect(socket_borrow, &io, &error)) {
    if (tcp_handle_error(socket, error) < 0) {
      if (error == NETWORK_ERROR_CODE_WOULD_BLOCK) {
	errno = EINPROGRESS;
      } else {
        socket->state.tag = TCP_SOCKET_STATE_CONNECT_FAILED;
        socket->state.connect_failed.error_code = error;
      }
      return -1;
    }
  }

  // Connect successful.
  streams_own_input_stream_t input = io.f0;
  streams_own_output_stream_t output = io.f1;

  socket->state.tag = TCP_SOCKET_STATE_CONNECTED;
  memset(&socket->state.connected, 0, sizeof(socket->state.connected));
  socket->state.connected.input = input;
  socket->state.connected.output = output;

  return 0;
}

static int tcp_getsockname(void *data, struct sockaddr *addr, socklen_t *addrlen) {
  tcp_socket_t *socket = (tcp_socket_t *)data;
  output_sockaddr_t output_addr;
  if (!__wasi_sockets_utils__output_addr_validate(socket->family, addr, addrlen, &output_addr)) {
    errno = EINVAL;
    return -1;
  }

  if (output_addr.tag == OUTPUT_SOCKADDR_NULL) {
    errno = EINVAL;
    return -1;
  }

  switch (socket->state.tag) {
    case TCP_SOCKET_STATE_UNBOUND:
      errno = EINVAL;
      return -1;

    case TCP_SOCKET_STATE_BOUND:
    case TCP_SOCKET_STATE_CONNECTING:
    case TCP_SOCKET_STATE_CONNECT_FAILED:
    case TCP_SOCKET_STATE_LISTENING:
    case TCP_SOCKET_STATE_CONNECTED:
      // OK. Continue..
      break;

    default: /* unreachable */
      abort();
  }

  network_error_code_t error;
  network_ip_socket_address_t result;
  tcp_borrow_tcp_socket_t socket_borrow = tcp_borrow_tcp_socket(socket->socket);
  if (!tcp_method_tcp_socket_local_address(socket_borrow, &result, &error)) {
    errno = __wasi_sockets_utils__map_error(error);
    return -1;
  }

  __wasi_sockets_utils__output_addr_write(result, &output_addr);
  return 0;
}

int tcp_getpeername(void *data, struct sockaddr *addr, socklen_t *addrlen) {
  tcp_socket_t *socket = (tcp_socket_t *)data;
  output_sockaddr_t output_addr;
  if (!__wasi_sockets_utils__output_addr_validate(
        socket->family, addr, addrlen, &output_addr)) {
    errno = EINVAL;
    return -1;
  }

  if (output_addr.tag == OUTPUT_SOCKADDR_NULL) {
    errno = EINVAL;
    return -1;
  }

  switch (socket->state.tag) {
    case TCP_SOCKET_STATE_UNBOUND:
    case TCP_SOCKET_STATE_BOUND:
    case TCP_SOCKET_STATE_CONNECTING:
    case TCP_SOCKET_STATE_CONNECT_FAILED:
    case TCP_SOCKET_STATE_LISTENING:
      errno = ENOTCONN;
      return -1;

    case TCP_SOCKET_STATE_CONNECTED:
      // OK. Continue..
      break;

    default: /* unreachable */
      abort();
  }

  network_error_code_t error;
  network_ip_socket_address_t result;
  tcp_borrow_tcp_socket_t socket_borrow = tcp_borrow_tcp_socket(socket->socket);
  if (!tcp_method_tcp_socket_remote_address(socket_borrow, &result, &error)) {
    errno = __wasi_sockets_utils__map_error(error);
    return -1;
  }

  __wasi_sockets_utils__output_addr_write(result, &output_addr);
  return 0;
}

static int tcp_listen(void *data, int backlog) {
  tcp_socket_t *socket = (tcp_socket_t *)data;
  network_error_code_t error;
  tcp_borrow_tcp_socket_t socket_borrow =
    tcp_borrow_tcp_socket(socket->socket);

  switch (socket->state.tag) {
    case TCP_SOCKET_STATE_UNBOUND: {
      // Socket is not explicitly bound by the user. We'll do it for them:

      network_ip_socket_address_t any = __wasi_sockets_utils__any_addr(socket->family);
      if (tcp_do_bind(socket, &any) < 0)
        return -1;

      if (socket->state.tag != TCP_SOCKET_STATE_BOUND) {
        abort();
      }
      // Great! We'll continue below.
      break;
    }
    case TCP_SOCKET_STATE_BOUND:
      // Great! We'll continue below.
      break;
    case TCP_SOCKET_STATE_LISTENING:
      // We can only update the backlog size.
      break;
    case TCP_SOCKET_STATE_CONNECTING:
    case TCP_SOCKET_STATE_CONNECTED:
    case TCP_SOCKET_STATE_CONNECT_FAILED:
    default:
      errno = EINVAL;
      return -1;
  }

  if (!tcp_method_tcp_socket_set_listen_backlog_size(socket_borrow, backlog, &error)) {
    errno = __wasi_sockets_utils__map_error(error);
    return -1;
  }

  if (socket->state.tag == TCP_SOCKET_STATE_LISTENING) {
    // Updating the backlog is all we had to do.
    return 0;
  }

  network_borrow_network_t network_borrow = __wasi_sockets_utils__borrow_network();
  if (!tcp_method_tcp_socket_start_listen(socket_borrow, &error)) {
    errno = __wasi_sockets_utils__map_error(error);
    return -1;
  }

  // Listen has successfully started. Attempt to finish it:
  while (!tcp_method_tcp_socket_finish_listen(socket_borrow, &error)) {
    if (tcp_handle_error(socket, error) < 0)
      return -1;
  }

  // Listen successful.
  socket->state.tag = TCP_SOCKET_STATE_LISTENING;
  return 0;
}

static ssize_t tcp_recvfrom(void *data, void *buffer,
			    size_t length, int flags, struct sockaddr *addr,
			    socklen_t *addrlen) {
  tcp_socket_t *socket = (tcp_socket_t *)data;
  // TODO wasi-sockets: flags:
  // - MSG_WAITALL: we can probably support these relatively easy.
  // - MSG_OOB: could be shimmed by always responding that no OOB data is available.
  // - MSG_PEEK: could be shimmed by performing the receive into a local socket-specific buffer. And on subsequent receives first check that buffer.

  const int supported_flags = MSG_DONTWAIT;
  if ((flags & supported_flags) != flags) {
    errno = EOPNOTSUPP;
    return -1;
  }

  if (addr != NULL || addrlen != NULL) {
    errno = EISCONN;
    return -1;
  }

  if (socket->state.tag != TCP_SOCKET_STATE_CONNECTED) {
    errno = ENOTCONN;
    return -1;
  }
  tcp_socket_state_connected_t *connection = &socket->state.connected;

  bool should_block = socket->blocking;
  if ((flags & MSG_DONTWAIT) != 0) {
    should_block = false;
  }

  streams_borrow_input_stream_t rx_borrow = streams_borrow_input_stream(connection->input);
  while (true) {
    wasip2_list_u8_t result;
    streams_stream_error_t error;
    if (!streams_method_input_stream_read(rx_borrow, length, &result, &error))
      // TODO wasi-sockets: wasi-sockets has no way to recover TCP stream errors yet.
      return wasip2_handle_read_error(error);

    if (result.len) {
      memcpy(buffer, result.ptr, result.len);
      wasip2_list_u8_free(&result);
      return result.len;
    }
    if (!should_block) {
      errno = EWOULDBLOCK;
      return -1;
    }

    if (connection->input_pollable.__handle == 0)
      connection->input_pollable = streams_method_input_stream_subscribe(rx_borrow);
    poll_borrow_pollable_t pollable_borrow = poll_borrow_pollable(connection->input_pollable);

    if (socket->recv_timeout != 0) {
      poll_own_pollable_t timeout_pollable =
        monotonic_clock_subscribe_duration(socket->recv_timeout);
      poll_list_borrow_pollable_t pollables;
      poll_borrow_pollable_t pollables_ptr[2];
      pollables.ptr = pollables_ptr;
      pollables.ptr[0] = pollable_borrow;
      pollables.ptr[1] = poll_borrow_pollable(timeout_pollable);
      pollables.len = 2;
      wasip2_list_u32_t ret;
      poll_poll(&pollables, &ret);
      poll_pollable_drop_own(timeout_pollable);
      for (size_t i = 0; i < ret.len; i++) {
        if (ret.ptr[i] == 1) {
          // Timed out
          errno = EWOULDBLOCK;
          wasip2_list_u32_free(&ret);
          return -1;
        }
      }
      wasip2_list_u32_free(&ret);
    } else {
      poll_method_pollable_block(pollable_borrow);
    }
  }
}

static ssize_t tcp_sendto(void *data, const void *buffer,
                          size_t length, int flags, const struct sockaddr *addr,
                          socklen_t addrlen) {
  tcp_socket_t *socket = (tcp_socket_t *)data;
  const int supported_flags = MSG_DONTWAIT | MSG_NOSIGNAL;
  if ((flags & supported_flags) != flags) {
    errno = EOPNOTSUPP;
    return -1;
  }

  if (addr != NULL || addrlen != 0) {
    errno = EISCONN;
    return -1;
  }

  tcp_socket_state_connected_t *connection;
  if (socket->state.tag == TCP_SOCKET_STATE_CONNECTED) {
    connection = &socket->state.connected;
  } else {
    errno = ENOTCONN;
    return -1;
  }

  bool should_block = socket->blocking;
  if ((flags & MSG_DONTWAIT) != 0) {
    should_block = false;
  }

  if ((flags & MSG_NOSIGNAL) != 0) {
    // Ignore it. WASI has no Unix-style signals. So effectively,
    // MSG_NOSIGNAL is always the case, whether it was explicitly
    // requested or not.
  }

  streams_borrow_output_stream_t tx_borrow = streams_borrow_output_stream(connection->output);
  while (true) {
    streams_stream_error_t error;
    uint64_t count;
    if (!streams_method_output_stream_check_write(tx_borrow, &count, &error))
      // TODO wasi-sockets: wasi-sockets has no way to recover stream errors yet.
      return wasip2_handle_write_error(error);

    if (count) {
      count = count < length ? count : length;
      wasip2_list_u8_t list = { .ptr = (uint8_t *)buffer, .len = count };
      if (!streams_method_output_stream_write(tx_borrow, &list, &error)) {
        // TODO wasi-sockets: wasi-sockets has no way to recover TCP stream errors yet.
        return wasip2_handle_write_error(error);
      } else {
        return count;
      }
    }
    if (!should_block) {
      errno = EWOULDBLOCK;
      return -1;
    }

    if (connection->output_pollable.__handle == 0)
      connection->output_pollable = streams_method_output_stream_subscribe(tx_borrow);
    poll_borrow_pollable_t pollable_borrow = poll_borrow_pollable(connection->output_pollable);

    if (socket->send_timeout != 0) {
      monotonic_clock_own_pollable_t timeout_pollable =
        monotonic_clock_subscribe_duration(socket->send_timeout);
      poll_list_borrow_pollable_t pollables;
      poll_borrow_pollable_t pollables_ptr[2];
      pollables.ptr = pollables_ptr;
      pollables.ptr[0] = pollable_borrow;
      pollables.ptr[1] = poll_borrow_pollable(timeout_pollable);
      pollables.len = 2;
      wasip2_list_u32_t ret;
      poll_poll(&pollables, &ret);
      poll_pollable_drop_own(timeout_pollable);
      for (size_t i = 0; i < ret.len; i++) {
        if (ret.ptr[i] == 1) {
          // Timed out
          errno = EWOULDBLOCK;
          wasip2_list_u32_free(&ret);
          return -1;
        }
      }
      wasip2_list_u32_free(&ret);
    } else {
      poll_method_pollable_block(pollable_borrow);
    }
  }
}

static int tcp_shutdown(void *data, int posix_how) {
  tcp_socket_t *socket = (tcp_socket_t *)data;
  tcp_shutdown_type_t wasi_how;
  switch (posix_how) {
    case SHUT_RD:
      wasi_how = TCP_SHUTDOWN_TYPE_RECEIVE;
      break;
    case SHUT_WR:
      wasi_how = TCP_SHUTDOWN_TYPE_SEND;
      break;
    case SHUT_RDWR:
      wasi_how = TCP_SHUTDOWN_TYPE_BOTH;
      break;
    default:
      errno = EINVAL;
      return -1;
  }

  tcp_socket_state_connected_t *connection;
  if (socket->state.tag == TCP_SOCKET_STATE_CONNECTED) {
    connection = &socket->state.connected;
  } else {
    errno = ENOTCONN;
    return -1;
  }

  network_error_code_t error;
  tcp_borrow_tcp_socket_t socket_borrow = tcp_borrow_tcp_socket(socket->socket);
  if (!tcp_method_tcp_socket_shutdown(socket_borrow, wasi_how, &error)) {
    errno = __wasi_sockets_utils__map_error(error);
    return -1;
  }

  if (posix_how == SHUT_RD || posix_how == SHUT_RDWR) {
    // TODO wasi-sockets: drop input stream (if not already). And
    // update `recv` to take dropped input streams into account.
  }

  if (posix_how == SHUT_WR || posix_how == SHUT_RDWR) {
    // TODO wasi-sockets: drop output stream (if not already). And
    // update `send` to take dropped output streams into account.
  }

  return 0;
}

static int tcp_getsockopt(void *data, int level, int optname,
		   void *restrict optval, socklen_t *restrict optlen) {
  tcp_socket_t *socket = (tcp_socket_t *)data;
  int value = 0;

  network_error_code_t error;
  tcp_borrow_tcp_socket_t socket_borrow = tcp_borrow_tcp_socket(socket->socket);

  switch (level) {
    case SOL_SOCKET:
      switch (optname) {
        case SO_TYPE:
          value = SOCK_STREAM;
          break;
        case SO_PROTOCOL:
          value = IPPROTO_TCP;
          break;

        case SO_DOMAIN:
          value = __wasi_sockets_utils__posix_family(socket->family);
          break;

        case SO_ERROR:
          if (socket->state.tag == TCP_SOCKET_STATE_CONNECT_FAILED) {
            value = __wasi_sockets_utils__map_error(socket->state.connect_failed.error_code);
            socket->state.connect_failed.error_code = 0;
          } else if (socket->state.tag == TCP_SOCKET_STATE_CONNECTING) {
            value = EINPROGRESS;
          } else {
            value = 0;
          }
          break;

        case SO_ACCEPTCONN: {
          bool is_listening = socket->state.tag == TCP_SOCKET_STATE_LISTENING;
          // Sanity check.
          if (is_listening != tcp_method_tcp_socket_is_listening(socket_borrow)) {
            abort();
          }
          value = is_listening;
          break;
        }
        case SO_KEEPALIVE: {
          bool result;
          if (!tcp_method_tcp_socket_keep_alive_enabled(socket_borrow, &result, &error)) {
            errno = __wasi_sockets_utils__map_error(error);
            return -1;
          }
          value = result;
          break;
        }
        case SO_RCVBUF: {
          uint64_t result;
          if (!tcp_method_tcp_socket_receive_buffer_size(socket_borrow, &result, &error)) {
            errno = __wasi_sockets_utils__map_error(error);
            return -1;
          }

          if (result > INT_MAX) {
            abort();
          }

          value = result;
          break;
        }
        case SO_SNDBUF: {
          uint64_t result;
          if (!tcp_method_tcp_socket_send_buffer_size(socket_borrow, &result, &error)) {
            errno = __wasi_sockets_utils__map_error(error);
            return -1;
          }

          if (result > INT_MAX) {
            abort();
          }

          value = result;
          break;
        }
        case SO_REUSEADDR:
          value = socket->fake_reuseaddr;
          break;
        case SO_RCVTIMEO: {
          struct timeval tv = duration_to_timeval(socket->recv_timeout);
          memcpy(optval, &tv, *optlen < sizeof(struct timeval) ? *optlen : sizeof(struct timeval));
          *optlen = sizeof(struct timeval);
          return 0;
        }
        case SO_SNDTIMEO: {
          struct timeval tv = duration_to_timeval(socket->send_timeout);
          memcpy(optval, &tv, *optlen < sizeof(struct timeval) ? *optlen : sizeof(struct timeval));
          *optlen = sizeof(struct timeval);
          return 0;
        }
        default:
          errno = ENOPROTOOPT;
          return -1;
      }
      break;

    case SOL_IP:
      switch (optname) {
        case IP_TTL: {
          if (socket->family != NETWORK_IP_ADDRESS_FAMILY_IPV4) {
            errno = EAFNOSUPPORT;
            return -1;
          }

          uint8_t result;
          if (!tcp_method_tcp_socket_hop_limit(socket_borrow, &result, &error)) {
            errno = __wasi_sockets_utils__map_error(error);
            return -1;
          }

          value = result;
          break;
        }
        default:
          errno = ENOPROTOOPT;
          return -1;
      }
      break;

    case SOL_IPV6:
      switch (optname) {
        case IPV6_UNICAST_HOPS: {
          if (socket->family != NETWORK_IP_ADDRESS_FAMILY_IPV6) {
            errno = EAFNOSUPPORT;
            return -1;
          }

          uint8_t result;
          if (!tcp_method_tcp_socket_hop_limit(socket_borrow, &result, &error)) {
            errno = __wasi_sockets_utils__map_error(error);
            return -1;
          }

          value = result;
          break;
        }
        default:
          errno = ENOPROTOOPT;
          return -1;
      }
      break;

    case SOL_TCP:
      switch (optname) {
        case TCP_NODELAY: {
          value = socket->fake_nodelay;
          break;
        }
        case TCP_KEEPIDLE: {
          tcp_duration_t result_ns;
          if (!tcp_method_tcp_socket_keep_alive_idle_time(
                socket_borrow, &result_ns, &error)) {
            errno = __wasi_sockets_utils__map_error(error);
            return -1;
          }

          uint64_t result_s = result_ns / NS_PER_S;
          if (result_s == 0) {
            result_s = 1; // Value was rounded down to zero. Round it up instead, because 0 is an invalid value for this socket option.
          }

          if (result_s > INT_MAX) {
            abort();
          }

          value = result_s;
          break;
        }
        case TCP_KEEPINTVL: {
          tcp_duration_t result_ns;
          if (!tcp_method_tcp_socket_keep_alive_interval(socket_borrow, &result_ns, &error)) {
            errno = __wasi_sockets_utils__map_error(error);
            return -1;
          }

          uint64_t result_s = result_ns / NS_PER_S;
          if (result_s == 0) {
            result_s = 1; // Value was rounded down to zero. Round it up instead, because 0 is an invalid value for this socket option.
          }

          if (result_s > INT_MAX) {
            abort();
          }

          value = result_s;
          break;
        }
        case TCP_KEEPCNT: {
          uint32_t result;
          if (!tcp_method_tcp_socket_keep_alive_count(socket_borrow, &result, &error)) {
            errno = __wasi_sockets_utils__map_error(error);
            return -1;
          }

          if (result > INT_MAX) {
            abort();
          }

          value = result;
          break;
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

  // Copy out integer value.
  memcpy(optval, &value, *optlen < sizeof(int) ? *optlen : sizeof(int));
  *optlen = sizeof(int);
  return 0;
}

int tcp_setsockopt(void *data, int level, int optname,
		   const void *optval, socklen_t optlen) {
  tcp_socket_t *socket = (tcp_socket_t *)data;
  int intval = *(int *)optval;

  network_error_code_t error;
  tcp_borrow_tcp_socket_t socket_borrow = tcp_borrow_tcp_socket(socket->socket);

  switch (level) {
    case SOL_SOCKET:
      switch (optname) {
        case SO_KEEPALIVE: {
          if (!tcp_method_tcp_socket_set_keep_alive_enabled(
                socket_borrow, intval != 0, &error)) {
            errno = __wasi_sockets_utils__map_error(error);
            return -1;
          }

          return 0;
        }
        case SO_RCVBUF: {
          if (!tcp_method_tcp_socket_set_receive_buffer_size(
                socket_borrow, intval, &error)) {
            errno = __wasi_sockets_utils__map_error(error);
            return -1;
          }

          return 0;
        }
        case SO_SNDBUF: {
          if (!tcp_method_tcp_socket_set_send_buffer_size(
                socket_borrow, intval, &error)) {
            errno = __wasi_sockets_utils__map_error(error);
            return -1;
          }

          return 0;
        }
        case SO_REUSEADDR: {
          // As of this writing, WASI has no support for changing SO_REUSEADDR
          // -- it's enabled by default and cannot be disabled.  To keep
          // applications happy, we pretend to support enabling and disabling
          // it.
          socket->fake_reuseaddr = (intval != 0);
          return 0;
        }
        case SO_RCVTIMEO:  {
          struct timeval* tv = (struct timeval*) optval;
          if (!timeval_to_duration(tv, &socket->recv_timeout)) {
            errno = EINVAL;
            return -1;
          }
          return 0;
        }
        case SO_SNDTIMEO: {
          struct timeval* tv = (struct timeval*) optval;
          if (!timeval_to_duration(tv, &socket->send_timeout)) {
            errno = EINVAL;
            return -1;
          }
          return 0;
        }
        default:
          errno = ENOPROTOOPT;
          return -1;
      }
      break;

    case SOL_IP:
      switch (optname) {
        case IP_TTL: {
          if (socket->family != NETWORK_IP_ADDRESS_FAMILY_IPV4) {
            errno = EAFNOSUPPORT;
            return -1;
          }

          if (intval < 0 || intval > UINT8_MAX) {
            errno = EINVAL;
            return -1;
          }

          if (!tcp_method_tcp_socket_set_hop_limit(socket_borrow, intval, &error)) {
            errno = __wasi_sockets_utils__map_error(error);
            return -1;
          }

          return 0;
        }
        default:
          errno = ENOPROTOOPT;
          return -1;
      }
      break;

    case SOL_IPV6:
      switch (optname) {
        case IPV6_UNICAST_HOPS: {
          if (socket->family != NETWORK_IP_ADDRESS_FAMILY_IPV6) {
            errno = EAFNOSUPPORT;
            return -1;
          }

          if (intval < 0 || intval > UINT8_MAX) {
            errno = EINVAL;
            return -1;
          }

          if (!tcp_method_tcp_socket_set_hop_limit(socket_borrow, intval, &error)) {
            errno = __wasi_sockets_utils__map_error(error);
            return -1;
          }

          return 0;
        }
        default:
          errno = ENOPROTOOPT;
          return -1;
      }
      break;

    case SOL_TCP:
      switch (optname) {
        case TCP_NODELAY: {
          // At the time of writing, WASI has no support for TCP_NODELAY.
          // Yet, many applications expect this option to be implemented.
          // To ensure those applications can run on WASI at all, we fake
          // support for it by recording the value, but not doing anything
          // with it.
          // If/when WASI adds true support, we can remove this workaround
          // and implement it properly. From the application's perspective
          // the "worst" thing that can then happen is that it automagically
          // becomes faster.
          socket->fake_nodelay = (intval != 0);
          return 0;
        }
        case TCP_KEEPIDLE: {
          tcp_duration_t duration = intval * NS_PER_S;
          if (!tcp_method_tcp_socket_set_keep_alive_idle_time(
                socket_borrow, duration, &error)) {
            errno = __wasi_sockets_utils__map_error(error);
            return -1;
          }

          return 0;
        }
        case TCP_KEEPINTVL: {
          tcp_duration_t duration = intval * NS_PER_S;
          if (!tcp_method_tcp_socket_set_keep_alive_interval(
                socket_borrow, duration, &error)) {
            errno = __wasi_sockets_utils__map_error(error);
            return -1;
          }

          return 0;
        }
        case TCP_KEEPCNT: {
          if (!tcp_method_tcp_socket_set_keep_alive_count(
                socket_borrow, intval, &error)) {
            errno = __wasi_sockets_utils__map_error(error);
            return -1;
          }

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

static int tcp_poll_register(void *data,
                             poll_state_t *state,
                             short events) {
  tcp_socket_t *socket = (tcp_socket_t *)data;
  switch (socket->state.tag) {
    case TCP_SOCKET_STATE_CONNECTING:
    case TCP_SOCKET_STATE_LISTENING: {
      if ((events & (POLLRDNORM | POLLWRNORM)) != 0)
        return __wasilibc_poll_add(state, events, tcp_pollable(socket));
      break;
    }

    case TCP_SOCKET_STATE_CONNECTED: {
      if ((events & POLLRDNORM) != 0) {
        if (__wasilibc_poll_add_input_stream(
              state,
              streams_borrow_input_stream(socket->state.connected.input),
              &socket->state.connected.input_pollable) < 0)
          return -1;
      }
      if ((events & POLLWRNORM) != 0) {
        if (__wasilibc_poll_add_output_stream(
              state,
              streams_borrow_output_stream(socket->state.connected.output),
              &socket->state.connected.output_pollable) < 0)
          return -1;
      }
      break;
    }

    case TCP_SOCKET_STATE_CONNECT_FAILED: {
      __wasilibc_poll_ready(state, events);
      break;
    }

    default:
      errno = ENOTSUP;
      return -1;
  }
  return 0;
}

static int tcp_poll_finish(void *data, poll_state_t *state, short events) {
  tcp_socket_t *socket = (tcp_socket_t *)data;

  if (socket->state.tag != TCP_SOCKET_STATE_CONNECTING) {
    __wasilibc_poll_ready(state, events);
    return 0;
  }

  tcp_borrow_tcp_socket_t borrow = tcp_borrow_tcp_socket(socket->socket);
  tcp_tuple2_own_input_stream_own_output_stream_t tuple;
  tcp_error_code_t error;
  if (tcp_method_tcp_socket_finish_connect(borrow, &tuple, &error)) {
    socket->state.tag = TCP_SOCKET_STATE_CONNECTED;
    memset(&socket->state.connected, 0, sizeof(socket->state.connected));
    socket->state.connected.input = tuple.f0;
    socket->state.connected.output = tuple.f1;
    __wasilibc_poll_ready(state, events);
  } else if (error == NETWORK_ERROR_CODE_WOULD_BLOCK) {
    // No events yet -- application will need to poll again
  } else {
    socket->state.tag = TCP_SOCKET_STATE_CONNECT_FAILED;
    socket->state.connect_failed.error_code = error;
    __wasilibc_poll_ready(state, events);
  }
  return 0;
}

static descriptor_vtable_t tcp_vtable = {
  .free = tcp_free,

  .get_read_stream = tcp_get_read_stream,
  .get_write_stream = tcp_get_write_stream,
  .set_blocking = tcp_set_blocking,
  .fstat = tcp_fstat,

  .accept4 = tcp_accept4,
  .bind = tcp_bind,
  .connect = tcp_connect,
  .getsockname = tcp_getsockname,
  .getpeername = tcp_getpeername,
  .listen = tcp_listen,
  .recvfrom = tcp_recvfrom,
  .sendto = tcp_sendto,
  .shutdown = tcp_shutdown,
  .getsockopt = tcp_getsockopt,
  .setsockopt = tcp_setsockopt,

  .poll_register = tcp_poll_register,
  .poll_finish = tcp_poll_finish,
};

#endif // __wasilibc_use_wasip2
