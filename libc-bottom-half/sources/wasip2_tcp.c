#include "common/time.h"
#include <errno.h>
#include <limits.h>
#include <netinet/tcp.h>
#include <stdlib.h>
#include <string.h>
#include <wasi/descriptor_table.h>
#include <wasi/file_utils.h>
#include <wasi/sockets_utils.h>
#include <wasi/tcp.h>
#include <wasi/wasip2.h>

#ifdef __wasip2__

static descriptor_vtable_t tcp_vtable;

static int tcp_add(tcp_own_tcp_socket_t socket,
                   network_ip_address_family_t family, bool blocking,
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

static int tcp_get_read_stream(void *data, wasip2_read_t *read) {
  tcp_socket_t *tcp = (tcp_socket_t *)data;

  if (tcp->state.tag != TCP_SOCKET_STATE_CONNECTED) {
    errno = ENOTCONN;
    return -1;
  }
  read->input = streams_borrow_input_stream(tcp->state.connected.input);
  read->offset = NULL;
  read->pollable = &tcp->state.connected.input_pollable;
  read->timeout = tcp->recv_timeout;
  read->blocking = tcp->blocking;
  return 0;
}

static int tcp_get_write_stream(void *data, wasip2_write_t *write) {
  tcp_socket_t *tcp = (tcp_socket_t *)data;

  if (tcp->state.tag != TCP_SOCKET_STATE_CONNECTED) {
    errno = ENOTCONN;
    return -1;
  }
  write->output = streams_borrow_output_stream(tcp->state.connected.output);
  write->offset = NULL;
  write->pollable = &tcp->state.connected.output_pollable;
  write->timeout = tcp->send_timeout;
  write->blocking = tcp->blocking;
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
    tcp_borrow_tcp_socket_t socket_borrow =
        tcp_borrow_tcp_socket(socket->socket);
    socket->socket_pollable = tcp_method_tcp_socket_subscribe(socket_borrow);
  }
  return poll_borrow_pollable(socket->socket_pollable);
}

static int tcp_handle_error(tcp_socket_t *socket, network_error_code_t error) {
  if (error == NETWORK_ERROR_CODE_WOULD_BLOCK && socket->blocking) {
    poll_method_pollable_block(tcp_pollable(socket));
  } else {
    return __wasilibc_socket_error_to_errno(error);
  }

  return 0;
}

static int tcp_accept4(void *data, struct sockaddr *addr, socklen_t *addrlen,
                       int flags) {
  tcp_socket_t *socket = (tcp_socket_t *)data;
  output_sockaddr_t output_addr;
  if (__wasilibc_sockaddr_validate(socket->family, addr, addrlen,
                                   &output_addr) < 0) {
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
    if (!tcp_method_tcp_socket_remote_address(client_borrow, &remote_address,
                                              &error)) {
      // TODO wasi-sockets: How to recover from this in a POSIX compatible way?
      abort();
    }

    __wasilibc_wasi_to_sockaddr(remote_address, &output_addr);
  }

  tcp_socket_t *client_socket;
  int client_fd = tcp_add(client, socket->family, (flags & SOCK_NONBLOCK) == 0,
                          &client_socket);
  if (client_fd < 0) {
    streams_input_stream_drop_own(input);
    streams_output_stream_drop_own(output);
    return -1;
  }

  client_socket->state.tag = TCP_SOCKET_STATE_CONNECTED;
  memset(&client_socket->state.connected, 0,
         sizeof(client_socket->state.connected));
  client_socket->state.connected.input = input;
  client_socket->state.connected.output = output;
  client_socket->fake_nodelay = socket->fake_nodelay;
  return client_fd;
}

static int tcp_do_bind(tcp_socket_t *socket,
                       network_ip_socket_address_t *address) {
  if (socket->state.tag != TCP_SOCKET_STATE_UNBOUND) {
    errno = EINVAL;
    return -1;
  }

  network_error_code_t error;
  network_borrow_network_t network_borrow =
      __wasi_sockets_utils__borrow_network();
  tcp_borrow_tcp_socket_t socket_borrow = tcp_borrow_tcp_socket(socket->socket);

  if (!tcp_method_tcp_socket_start_bind(socket_borrow, network_borrow, address,
                                        &error)) {
    return __wasilibc_socket_error_to_errno(error);
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

static int tcp_bind(void *data, const struct sockaddr *addr,
                    socklen_t addrlen) {
  tcp_socket_t *socket = (tcp_socket_t *)data;
  network_ip_socket_address_t local_address;
  if (__wasilibc_sockaddr_to_wasi(socket->family, addr, addrlen,
                                  &local_address) < 0)
    return -1;
  return tcp_do_bind(socket, &local_address);
}

static int tcp_connect(void *data, const struct sockaddr *addr,
                       socklen_t addrlen) {
  tcp_socket_t *socket = (tcp_socket_t *)data;
  network_ip_socket_address_t remote_address;
  if (__wasilibc_sockaddr_to_wasi(socket->family, addr, addrlen,
                                  &remote_address) < 0)
    return -1;

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
  case TCP_SOCKET_STATE_CONNECT_FAILED: // POSIX: "If connect() fails, the state
                                        // of the socket is unspecified.
                                        // Conforming applications should close
                                        // the file descriptor and create a new
                                        // socket before attempting to
                                        // reconnect."
  case TCP_SOCKET_STATE_LISTENING:
  default:
    errno = EOPNOTSUPP;
    return -1;
  }

  network_error_code_t error;
  network_borrow_network_t network_borrow =
      __wasi_sockets_utils__borrow_network();
  tcp_borrow_tcp_socket_t socket_borrow = tcp_borrow_tcp_socket(socket->socket);

  if (!tcp_method_tcp_socket_start_connect(socket_borrow, network_borrow,
                                           &remote_address, &error)) {
    return __wasilibc_socket_error_to_errno(error);
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

static int tcp_getsockname(void *data, struct sockaddr *addr,
                           socklen_t *addrlen) {
  tcp_socket_t *socket = (tcp_socket_t *)data;
  output_sockaddr_t output_addr;
  if (__wasilibc_sockaddr_validate(socket->family, addr, addrlen,
                                   &output_addr) < 0)
    return -1;

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
  if (!tcp_method_tcp_socket_local_address(socket_borrow, &result, &error))
    return __wasilibc_socket_error_to_errno(error);

  __wasilibc_wasi_to_sockaddr(result, &output_addr);
  return 0;
}

int tcp_getpeername(void *data, struct sockaddr *addr, socklen_t *addrlen) {
  tcp_socket_t *socket = (tcp_socket_t *)data;
  output_sockaddr_t output_addr;
  if (__wasilibc_sockaddr_validate(socket->family, addr, addrlen,
                                   &output_addr) < 0)
    return -1;

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
  if (!tcp_method_tcp_socket_remote_address(socket_borrow, &result, &error))
    return __wasilibc_socket_error_to_errno(error);

  __wasilibc_wasi_to_sockaddr(result, &output_addr);
  return 0;
}

static int tcp_listen(void *data, int backlog) {
  tcp_socket_t *socket = (tcp_socket_t *)data;
  network_error_code_t error;
  tcp_borrow_tcp_socket_t socket_borrow = tcp_borrow_tcp_socket(socket->socket);

  switch (socket->state.tag) {
  case TCP_SOCKET_STATE_UNBOUND: {
    // Socket is not explicitly bound by the user. We'll do it for them:

    network_ip_socket_address_t any;
    __wasilibc_unspecified_addr(socket->family, &any);
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

  if (!tcp_method_tcp_socket_set_listen_backlog_size(socket_borrow, backlog,
                                                     &error)) {
    return __wasilibc_socket_error_to_errno(error);
  }

  if (socket->state.tag == TCP_SOCKET_STATE_LISTENING) {
    // Updating the backlog is all we had to do.
    return 0;
  }

  network_borrow_network_t network_borrow =
      __wasi_sockets_utils__borrow_network();
  if (!tcp_method_tcp_socket_start_listen(socket_borrow, &error)) {
    return __wasilibc_socket_error_to_errno(error);
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

static ssize_t tcp_recvfrom(void *data, void *buffer, size_t length, int flags,
                            struct sockaddr *addr, socklen_t *addrlen) {
  tcp_socket_t *socket = (tcp_socket_t *)data;
  // TODO wasi-sockets: flags:
  // - MSG_WAITALL: we can probably support these relatively easy.
  // - MSG_OOB: could be shimmed by always responding that no OOB data is
  // available.
  // - MSG_PEEK: could be shimmed by performing the receive into a local
  // socket-specific buffer. And on subsequent receives first check that buffer.

  const int supported_flags = MSG_DONTWAIT;
  if ((flags & supported_flags) != flags) {
    errno = EOPNOTSUPP;
    return -1;
  }

  if (addr != NULL || addrlen != NULL) {
    errno = EISCONN;
    return -1;
  }

  wasip2_read_t read;
  if (tcp_get_read_stream(data, &read) < 0)
    return -1;

  if ((flags & MSG_DONTWAIT) != 0)
    read.blocking = false;

  return __wasilibc_read(&read, buffer, length);
}

static ssize_t tcp_sendto(void *data, const void *buffer, size_t length,
                          int flags, const struct sockaddr *addr,
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

  wasip2_write_t write;
  if (tcp_get_write_stream(data, &write) < 0)
    return -1;

  if ((flags & MSG_DONTWAIT) != 0)
    write.blocking = false;

  if ((flags & MSG_NOSIGNAL) != 0) {
    // Ignore it. WASI has no Unix-style signals. So effectively,
    // MSG_NOSIGNAL is always the case, whether it was explicitly
    // requested or not.
  }

  return __wasilibc_write(&write, buffer, length);
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
    return __wasilibc_socket_error_to_errno(error);
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

static int tcp_poll_register(void *data, poll_state_t *state, short events) {
  tcp_socket_t *socket = (tcp_socket_t *)data;
  switch (socket->state.tag) {
  case TCP_SOCKET_STATE_CONNECTING: {
    if ((events & (POLLRDNORM | POLLWRNORM)) != 0)
      return __wasilibc_poll_add(state, events, tcp_pollable(socket));
    break;
  }

  case TCP_SOCKET_STATE_LISTENING: {
    // Listening sockets can only be ready to read, not write.
    if ((events & POLLRDNORM) != 0)
      return __wasilibc_poll_add(state, events, tcp_pollable(socket));
    break;
  }

  case TCP_SOCKET_STATE_CONNECTED: {
    if ((events & POLLRDNORM) != 0) {
      if (__wasilibc_poll_add_input_stream(
              state, streams_borrow_input_stream(socket->state.connected.input),
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

  switch (socket->state.tag) {
  case TCP_SOCKET_STATE_CONNECTING:
    break;
  case TCP_SOCKET_STATE_LISTENING:
    // Listening sockets can only be ready to read, not write.
    __wasilibc_poll_ready(state, events & POLLRDNORM);
    return 0;
  default:
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
    // Now that it's connected, it's immediately writable but not necessarily
    // immediately readable:
    __wasilibc_poll_ready(state, events & POLLWRNORM);
  } else if (error == NETWORK_ERROR_CODE_WOULD_BLOCK) {
    // No events yet -- application will need to poll again
  } else {
    socket->state.tag = TCP_SOCKET_STATE_CONNECT_FAILED;
    socket->state.connect_failed.error_code = error;
    __wasilibc_poll_ready(state, events);
  }
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
    .getsockopt = __wasilibc_tcp_getsockopt,
    .setsockopt = __wasilibc_tcp_setsockopt,

    .poll_register = tcp_poll_register,
    .poll_finish = tcp_poll_finish,

    .fcntl_getfl = tcp_fcntl_getfl,
    .fcntl_setfl = tcp_fcntl_setfl,
};

#endif // __wasip2__
