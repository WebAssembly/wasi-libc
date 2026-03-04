#include <wasi/api.h>

#ifndef __wasip1__

#include <common/time.h>
#include <errno.h>
#include <limits.h>
#include <netinet/tcp.h>
#include <stdlib.h>
#include <string.h>
#include <wasi/descriptor_table.h>
#include <wasi/file_utils.h>
#include <wasi/sockets_utils.h>
#include <wasi/tcp.h>
#include <wasi/wasip3_block.h>

// Normalize names on WASIp2 to the WASIp3-based names
#ifdef __wasip2__

#include <wasi/wasip2.h>

#define sockets_method_tcp_socket_get_is_listening                             \
  tcp_method_tcp_socket_is_listening
#define sockets_method_tcp_socket_get_keep_alive_enabled                       \
  tcp_method_tcp_socket_keep_alive_enabled
#define sockets_method_tcp_socket_get_receive_buffer_size                      \
  tcp_method_tcp_socket_receive_buffer_size
#define sockets_method_tcp_socket_get_send_buffer_size                         \
  tcp_method_tcp_socket_send_buffer_size
#define sockets_method_tcp_socket_set_keep_alive_enabled                       \
  tcp_method_tcp_socket_set_keep_alive_enabled
#define sockets_method_tcp_socket_set_receive_buffer_size                      \
  tcp_method_tcp_socket_set_receive_buffer_size
#define sockets_method_tcp_socket_set_send_buffer_size                         \
  tcp_method_tcp_socket_set_send_buffer_size
#define sockets_method_tcp_socket_get_hop_limit tcp_method_tcp_socket_hop_limit
#define sockets_method_tcp_socket_set_hop_limit                                \
  tcp_method_tcp_socket_set_hop_limit
#define sockets_method_tcp_socket_get_keep_alive_idle_time                     \
  tcp_method_tcp_socket_keep_alive_idle_time
#define sockets_method_tcp_socket_get_keep_alive_interval                      \
  tcp_method_tcp_socket_keep_alive_interval
#define sockets_method_tcp_socket_get_keep_alive_count                         \
  tcp_method_tcp_socket_keep_alive_count
#define sockets_method_tcp_socket_set_keep_alive_idle_time                     \
  tcp_method_tcp_socket_set_keep_alive_idle_time
#define sockets_method_tcp_socket_set_keep_alive_interval                      \
  tcp_method_tcp_socket_set_keep_alive_interval
#define sockets_method_tcp_socket_set_keep_alive_count                         \
  tcp_method_tcp_socket_set_keep_alive_count
#define sockets_method_tcp_socket_get_remote_address                           \
  tcp_method_tcp_socket_remote_address
#define sockets_method_tcp_socket_get_local_address                            \
  tcp_method_tcp_socket_local_address
#define sockets_method_tcp_socket_set_listen_backlog_size                      \
  tcp_method_tcp_socket_set_listen_backlog_size

#define sockets_borrow_tcp_socket tcp_borrow_tcp_socket
#define sockets_borrow_tcp_socket_t tcp_borrow_tcp_socket_t
#define sockets_own_tcp_socket_t tcp_own_tcp_socket_t
#define sockets_tcp_socket_drop_own tcp_tcp_socket_drop_own

typedef tcp_duration_t sockets_duration_t;
#endif // __wasip2__

static const uint64_t NS_PER_S = 1000000000;

static descriptor_vtable_t tcp_vtable;

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

static void tcp_free(void *data) {
  tcp_socket_t *tcp = (tcp_socket_t *)data;

  switch (tcp->state.tag) {
#ifdef __wasip3__
  case TCP_SOCKET_STATE_CONNECTING: {
    tcp_socket_state_connecting_t *state = &tcp->state.connecting;
    if (state->subtask != 0) {
      wasip3_subtask_cancel(state->subtask);
      wasip3_subtask_drop(state->subtask);
    }
    break;
  }

  case TCP_SOCKET_STATE_LISTENING: {
    tcp_socket_state_listening_t *state = &tcp->state.listening;
    if (state->stream != 0)
      sockets_stream_own_tcp_socket_drop_readable(state->stream);
    break;
  }
#endif

  case TCP_SOCKET_STATE_CONNECTED: {
    tcp_socket_state_connected_t *state = &tcp->state.connected;

#ifdef __wasip2__
    if (state->input_pollable.__handle != 0)
      poll_pollable_drop_own(state->input_pollable);
    if (state->output_pollable.__handle != 0)
      poll_pollable_drop_own(state->output_pollable);
    streams_input_stream_drop_own(state->input);
    streams_output_stream_drop_own(state->output);
#else
    if (state->receive != 0)
      sockets_stream_u8_drop_readable(state->receive);
    if (state->receive_result != 0)
      sockets_future_result_void_error_code_drop_readable(
          state->receive_result);
    if (state->send != 0)
      sockets_stream_u8_drop_writable(state->send);
    // TODO: when wasmtime 43.0.0 is released this should be changed to
    // cancelling the subtask and dropping it, but that doesn't work well in 42
    // which is currently released.
    if (state->send_subtask != 0)
      __wasilibc_subtask_block_on_and_drop(state->send_subtask);
#endif
    break;
  }

  default:
    break;
  }

#ifdef __wasip2__
  if (tcp->socket_pollable.__handle != 0)
    poll_pollable_drop_own(tcp->socket_pollable);
#endif // __wasip2__
  sockets_tcp_socket_drop_own(tcp->socket);

  free(tcp);
}

#ifndef __wasip2__
static int tcp_read_eof(void *data) {
  tcp_socket_t *tcp = (tcp_socket_t *)data;
  assert(tcp->state.tag == TCP_SOCKET_STATE_CONNECTED);
  tcp_socket_state_connected_t *state = &tcp->state.connected;

  if (state->receive_result) {
    assert(tcp->blocking);
    sockets_result_void_error_code_t result;
    __wasilibc_future_block_on(sockets_future_result_void_error_code_read(
                                   state->receive_result, &result),
                               state->receive_result);
    if (result.is_err)
      return __wasilibc_socket_error_to_errno(result.val.err);
  }
  return 0;
}
#endif

static int tcp_get_read_stream(void *data, wasi_read_t *read) {
  tcp_socket_t *tcp = (tcp_socket_t *)data;

  if (tcp->state.tag != TCP_SOCKET_STATE_CONNECTED) {
    errno = ENOTCONN;
    return -1;
  }
  tcp_socket_state_connected_t *state = &tcp->state.connected;
#ifdef __wasip2__
  read->input = streams_borrow_input_stream(state->input);
  read->pollable = &state->input_pollable;
#else
  read->stream = state->receive;
  read->eof = tcp_read_eof;
  read->eof_data = data;
#endif
  read->offset = NULL;
  read->timeout = tcp->recv_timeout;
  read->blocking = tcp->blocking;
  return 0;
}

#ifndef __wasip2__
static int tcp_write_eof(void *data) {
  tcp_socket_t *tcp = (tcp_socket_t *)data;
  assert(tcp->state.tag == TCP_SOCKET_STATE_CONNECTED);
  tcp_socket_state_connected_t *state = &tcp->state.connected;

  if (state->send_subtask) {
    __wasilibc_subtask_block_on_and_drop(state->send_subtask);
    state->send_subtask = 0;
  }
  if (state->send_result.is_err)
    return __wasilibc_socket_error_to_errno(state->send_result.val.err);
  return 0;
}
#endif

static int tcp_get_write_stream(void *data, wasi_write_t *write) {
  tcp_socket_t *tcp = (tcp_socket_t *)data;

  if (tcp->state.tag != TCP_SOCKET_STATE_CONNECTED) {
    errno = ENOTCONN;
    return -1;
  }
  tcp_socket_state_connected_t *state = &tcp->state.connected;
#ifdef __wasip2__
  write->output = streams_borrow_output_stream(state->output);
  write->pollable = &state->output_pollable;
#else
  write->output = state->send;
  write->eof = tcp_write_eof;
  write->eof_data = data;
  write->done = &state->send_done;
#endif
  write->offset = NULL;
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
  (void)data;
  memset(buf, 0, sizeof(struct stat));
  buf->st_mode = S_IFSOCK;
  return 0;
}

#ifdef __wasip2__
static poll_borrow_pollable_t tcp_pollable(tcp_socket_t *socket) {
  if (socket->socket_pollable.__handle == 0) {
    sockets_borrow_tcp_socket_t socket_borrow =
        sockets_borrow_tcp_socket(socket->socket);
    socket->socket_pollable = tcp_method_tcp_socket_subscribe(socket_borrow);
  }
  return poll_borrow_pollable(socket->socket_pollable);
}

static int tcp_handle_error(tcp_socket_t *socket, sockets_error_code_t error) {
  if (error == NETWORK_ERROR_CODE_WOULD_BLOCK && socket->blocking) {
    poll_method_pollable_block(tcp_pollable(socket));
  } else {
    return __wasilibc_socket_error_to_errno(error);
  }

  return 0;
}

#else

// Setup the `TCP_SOCKET_STATE_CONNECTED` fields for a wasip3-connected socket.
static void tcp_setup_connected_state_wasip3(tcp_socket_t *socket) {
  sockets_borrow_tcp_socket_t socket_borrow =
      sockets_borrow_tcp_socket(socket->socket);

  socket->state.tag = TCP_SOCKET_STATE_CONNECTED;
  tcp_socket_state_connected_t *state = &socket->state.connected;

  sockets_tuple2_stream_u8_future_result_void_error_code_t receive_result;
  sockets_method_tcp_socket_receive(socket_borrow, &receive_result);
  state->receive = receive_result.f0;
  state->receive_result = receive_result.f1;

  state->send_done = false;
  sockets_stream_u8_t reader = sockets_stream_u8_new(&state->send);
  wasip3_subtask_status_t status = sockets_method_tcp_socket_send(
      socket_borrow, reader, &state->send_result);
  if (WASIP3_SUBTASK_STATE(status) != WASIP3_SUBTASK_RETURNED)
    state->send_subtask = WASIP3_SUBTASK_HANDLE(status);
}
#endif

static int tcp_accept4(void *data, struct sockaddr *addr, socklen_t *addrlen,
                       int flags) {
  tcp_socket_t *socket = (tcp_socket_t *)data;
  output_sockaddr_t output_addr;
  if (__wasilibc_sockaddr_validate(socket->family, addr, addrlen,
                                   &output_addr) < 0) {
    return -1;
  }

  if (socket->state.tag != TCP_SOCKET_STATE_LISTENING) {
    errno = EINVAL;
    return -1;
  }

  tcp_socket_t *client_socket;
  sockets_error_code_t error;

#ifdef __wasip2__
  sockets_borrow_tcp_socket_t socket_borrow =
      sockets_borrow_tcp_socket(socket->socket);
  tcp_tuple3_own_tcp_socket_own_input_stream_own_output_stream_t client_and_io;
  while (!tcp_method_tcp_socket_accept(socket_borrow, &client_and_io, &error)) {
    if (tcp_handle_error(socket, error) < 0)
      return -1;
  }

  sockets_own_tcp_socket_t client = client_and_io.f0;
  streams_own_input_stream_t input = client_and_io.f1;
  streams_own_output_stream_t output = client_and_io.f2;

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
#else
  tcp_socket_state_listening_t *state = &socket->state.listening;
  assert(socket->blocking);
  sockets_own_tcp_socket_t result;
  while (1) {
    // It's not clear what the correct error code to return here is, if any,
    // since in theory sockets being accepted are infinite until the socket is
    // closed. Handle this with at least some error for now.
    if (state->done) {
      errno = ENOTSUP;
      return -1;
    }

    // Wait on a socket to be accepted, and then break out of the loop. Turn
    // again on 0-length reads.
    size_t amt = __wasilibc_stream_block_on(
        sockets_stream_own_tcp_socket_read(state->stream, &result, 1),
        state->stream, &state->done);
    if (amt > 0)
      break;
  }
  int client_fd = tcp_add(result, socket->family, (flags & SOCK_NONBLOCK) == 0,
                          &client_socket);
  if (client_fd < 0)
    return -1;
  tcp_setup_connected_state_wasip3(client_socket);
#endif

  sockets_borrow_tcp_socket_t client_borrow =
      sockets_borrow_tcp_socket(client_socket->socket);
  if (output_addr.tag != OUTPUT_SOCKADDR_NULL) {
    sockets_ip_socket_address_t remote_address;
    if (!sockets_method_tcp_socket_get_remote_address(
            client_borrow, &remote_address, &error)) {
      // TODO wasi-sockets: How to recover from this in a POSIX compatible way?
      abort();
    }

    __wasilibc_wasi_to_sockaddr(remote_address, &output_addr);
  }
  client_socket->fake_nodelay = socket->fake_nodelay;
  return client_fd;
}

static int tcp_do_bind(tcp_socket_t *socket,
                       sockets_ip_socket_address_t *address) {
  if (socket->state.tag != TCP_SOCKET_STATE_UNBOUND) {
    errno = EINVAL;
    return -1;
  }

  sockets_error_code_t error;
  sockets_borrow_tcp_socket_t socket_borrow =
      sockets_borrow_tcp_socket(socket->socket);

#if defined(__wasip2__)
  network_borrow_network_t network_borrow =
      __wasi_sockets_utils__borrow_network();

  if (!tcp_method_tcp_socket_start_bind(socket_borrow, network_borrow, address,
                                        &error)) {
    return __wasilibc_socket_error_to_errno(error);
  }

  // Bind has successfully started. Attempt to finish it:
  while (!tcp_method_tcp_socket_finish_bind(socket_borrow, &error)) {
    if (tcp_handle_error(socket, error) < 0)
      return -1;
  }
#elif defined(__wasip3__)
  if (!sockets_method_tcp_socket_bind(socket_borrow, address, &error)) {
    return __wasilibc_socket_error_to_errno(error);
  }
#endif

  // Bind successful.
  socket->state.tag = TCP_SOCKET_STATE_BOUND;
  return 0;
}

static int tcp_bind(void *data, const struct sockaddr *addr,
                    socklen_t addrlen) {
  tcp_socket_t *socket = (tcp_socket_t *)data;
  sockets_ip_socket_address_t local_address;
  if (__wasilibc_sockaddr_to_wasi(socket->family, addr, addrlen,
                                  &local_address) < 0)
    return -1;
  return tcp_do_bind(socket, &local_address);
}

static int tcp_connect(void *data, const struct sockaddr *addr,
                       socklen_t addrlen) {
  tcp_socket_t *socket = (tcp_socket_t *)data;
  sockets_ip_socket_address_t remote_address;
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

  sockets_borrow_tcp_socket_t socket_borrow =
      sockets_borrow_tcp_socket(socket->socket);

#ifdef __wasip2__
  sockets_error_code_t error;
  network_borrow_network_t network_borrow =
      __wasi_sockets_utils__borrow_network();

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
#else
  // Setup the arguments to the `connect` function as well as initializing the
  // `subtask` field of the `connecting` state to zero as we're not sure we'll
  // get a subtask just yet.
  socket->state.connecting.args.self = socket_borrow;
  socket->state.connecting.args.remote_address = remote_address;
  socket->state.connecting.subtask = 0;

  wasip3_subtask_status_t status = sockets_method_tcp_socket_connect(
      &socket->state.connecting.args, &socket->state.connecting.result);

  // If the subtask hasn't returned yet then its arguments/results are stored
  // in the `TCP_SOCKET_STATE_CONNECTING` state that we're in right now, as
  // stable addresses. Here if the socket is in blocking mode we block on the
  // result of the task, and otherwise this returns that the connect is in
  // progress and otherwise bails out.
  if (WASIP3_SUBTASK_STATE(status) != WASIP3_SUBTASK_RETURNED) {
    wasip3_subtask_t subtask = WASIP3_SUBTASK_HANDLE(status);
    if (socket->blocking) {
      __wasilibc_subtask_block_on_and_drop(subtask);
    } else {
      socket->state.connecting.subtask = subtask;
      errno = EINPROGRESS;
      return -1;
    }
  }

  // The connect subtask has completed at this point, so check to see what the
  // result was.
  assert(socket->state.connecting.subtask == 0);
  if (socket->state.connecting.result.is_err) {
    sockets_error_code_t error = socket->state.connecting.result.val.err;
    __wasilibc_socket_error_to_errno(error);
    socket->state.tag = TCP_SOCKET_STATE_CONNECT_FAILED;
    socket->state.connect_failed.error_code = error;
    return -1;
  }

  // Our socket is now connected, so establish the send/receive streams that
  // will be used for data transmission.
  tcp_setup_connected_state_wasip3(socket);
#endif

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

  sockets_error_code_t error;
  sockets_ip_socket_address_t result;
  sockets_borrow_tcp_socket_t socket_borrow =
      sockets_borrow_tcp_socket(socket->socket);
  if (!sockets_method_tcp_socket_get_local_address(socket_borrow, &result,
                                                   &error))
    return __wasilibc_socket_error_to_errno(error);

  __wasilibc_wasi_to_sockaddr(result, &output_addr);
  return 0;
}

static int tcp_getpeername(void *data, struct sockaddr *addr,
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

  sockets_error_code_t error;
  sockets_ip_socket_address_t result;
  sockets_borrow_tcp_socket_t socket_borrow =
      sockets_borrow_tcp_socket(socket->socket);
  if (!sockets_method_tcp_socket_get_remote_address(socket_borrow, &result,
                                                    &error))
    return __wasilibc_socket_error_to_errno(error);

  __wasilibc_wasi_to_sockaddr(result, &output_addr);
  return 0;
}

static int tcp_listen(void *data, int backlog) {
  tcp_socket_t *socket = (tcp_socket_t *)data;
  sockets_error_code_t error;
  sockets_borrow_tcp_socket_t socket_borrow =
      sockets_borrow_tcp_socket(socket->socket);

  switch (socket->state.tag) {
  case TCP_SOCKET_STATE_UNBOUND: {
    // Socket is not explicitly bound by the user. We'll do it for them:

    sockets_ip_socket_address_t any;
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

  if (!sockets_method_tcp_socket_set_listen_backlog_size(socket_borrow, backlog,
                                                         &error)) {
    return __wasilibc_socket_error_to_errno(error);
  }

  if (socket->state.tag == TCP_SOCKET_STATE_LISTENING) {
    // Updating the backlog is all we had to do.
    return 0;
  }

#ifdef __wasip2__
  if (!tcp_method_tcp_socket_start_listen(socket_borrow, &error)) {
    return __wasilibc_socket_error_to_errno(error);
  }

  // Listen has successfully started. Attempt to finish it:
  while (!tcp_method_tcp_socket_finish_listen(socket_borrow, &error)) {
    if (tcp_handle_error(socket, error) < 0)
      return -1;
  }

  socket->state.tag = TCP_SOCKET_STATE_LISTENING;
#else
  sockets_stream_own_tcp_socket_t stream;
  if (!sockets_method_tcp_socket_listen(socket_borrow, &stream, &error))
    return __wasilibc_socket_error_to_errno(error);

  socket->state.tag = TCP_SOCKET_STATE_LISTENING;
  socket->state.listening.stream = stream;
  socket->state.listening.done = false;
#endif

  return 0;
}

static ssize_t tcp_recvfrom(void *data, void *buffer, size_t length, int flags,
                            struct sockaddr *addr, socklen_t *addrlen) {
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

  wasi_read_t read;
  if (tcp_get_read_stream(data, &read) < 0)
    return -1;

  if ((flags & MSG_DONTWAIT) != 0)
    read.blocking = false;

  return __wasilibc_read(&read, buffer, length);
}

static ssize_t tcp_sendto(void *data, const void *buffer, size_t length,
                          int flags, const struct sockaddr *addr,
                          socklen_t addrlen) {
  const int supported_flags = MSG_DONTWAIT | MSG_NOSIGNAL;
  if ((flags & supported_flags) != flags) {
    errno = EOPNOTSUPP;
    return -1;
  }

  if (addr != NULL || addrlen != 0) {
    errno = EISCONN;
    return -1;
  }

  wasi_write_t write;
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

  if (socket->state.tag != TCP_SOCKET_STATE_CONNECTED) {
    errno = ENOTCONN;
    return -1;
  }

#ifdef __wasip2__
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

  sockets_error_code_t error;
  sockets_borrow_tcp_socket_t socket_borrow =
      sockets_borrow_tcp_socket(socket->socket);
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
#else
  tcp_socket_state_connected_t *state = &socket->state.connected;
  if (posix_how == SHUT_RD || posix_how == SHUT_RDWR) {
    if (state->receive != 0) {
      sockets_stream_u8_drop_readable(state->receive);
      state->receive = 0;
    }
  }

  if (posix_how == SHUT_WR || posix_how == SHUT_RDWR) {
    if (state->send != 0) {
      sockets_stream_u8_drop_writable(state->send);
      state->send = 0;
      state->send_done = true;
    }
  }
#endif

  return 0;
}

#ifdef __wasip2__
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

  sockets_borrow_tcp_socket_t borrow =
      sockets_borrow_tcp_socket(socket->socket);
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
#endif // __wasip2__

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

static int tcp_getsockopt(void *data, int level, int optname,
                          void *restrict optval, socklen_t *restrict optlen) {
  tcp_socket_t *socket = (tcp_socket_t *)data;
  int value = 0;

  sockets_error_code_t error;
  sockets_borrow_tcp_socket_t socket_borrow =
      sockets_borrow_tcp_socket(socket->socket);

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
      value = __wasilibc_wasi_family_to_libc(socket->family);
      break;

    case SO_ERROR:
      if (socket->state.tag == TCP_SOCKET_STATE_CONNECT_FAILED) {
        value = __wasilibc_map_socket_error(
            socket->state.connect_failed.error_code);
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
      if (is_listening !=
          sockets_method_tcp_socket_get_is_listening(socket_borrow)) {
        abort();
      }
      value = is_listening;
      break;
    }
    case SO_KEEPALIVE: {
      bool result;
      if (!sockets_method_tcp_socket_get_keep_alive_enabled(socket_borrow,
                                                            &result, &error))
        return __wasilibc_socket_error_to_errno(error);
      value = result;
      break;
    }
    case SO_RCVBUF: {
      uint64_t result;
      if (!sockets_method_tcp_socket_get_receive_buffer_size(socket_borrow,
                                                             &result, &error))
        return __wasilibc_socket_error_to_errno(error);

      value = result > INT_MAX ? INT_MAX : result;
      break;
    }
    case SO_SNDBUF: {
      uint64_t result;
      if (!sockets_method_tcp_socket_get_send_buffer_size(socket_borrow,
                                                          &result, &error))
        return __wasilibc_socket_error_to_errno(error);

      value = result > INT_MAX ? INT_MAX : result;
      break;
    }
    case SO_REUSEADDR:
      value = socket->fake_reuseaddr;
      break;
    case SO_RCVTIMEO: {
      struct timeval tv = duration_to_timeval(socket->recv_timeout);
      memcpy(optval, &tv,
             *optlen < sizeof(struct timeval) ? *optlen
                                              : sizeof(struct timeval));
      *optlen = sizeof(struct timeval);
      return 0;
    }
    case SO_SNDTIMEO: {
      struct timeval tv = duration_to_timeval(socket->send_timeout);
      memcpy(optval, &tv,
             *optlen < sizeof(struct timeval) ? *optlen
                                              : sizeof(struct timeval));
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
      if (socket->family != SOCKETS_IP_ADDRESS_FAMILY_IPV4) {
        errno = EAFNOSUPPORT;
        return -1;
      }

      uint8_t result;
      if (!sockets_method_tcp_socket_get_hop_limit(socket_borrow, &result,
                                                   &error))
        return __wasilibc_socket_error_to_errno(error);

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
      if (socket->family != SOCKETS_IP_ADDRESS_FAMILY_IPV6) {
        errno = EAFNOSUPPORT;
        return -1;
      }

      uint8_t result;
      if (!sockets_method_tcp_socket_get_hop_limit(socket_borrow, &result,
                                                   &error))
        return __wasilibc_socket_error_to_errno(error);

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
      sockets_duration_t result_ns;
      if (!sockets_method_tcp_socket_get_keep_alive_idle_time(
              socket_borrow, &result_ns, &error))
        return __wasilibc_socket_error_to_errno(error);

      uint64_t result_s = result_ns / NS_PER_S;
      if (result_s == 0) {
        result_s = 1; // Value was rounded down to zero. Round it up instead,
                      // because 0 is an invalid value for this socket option.
      }

      value = result_s > INT_MAX ? INT_MAX : result_s;
      break;
    }
    case TCP_KEEPINTVL: {
      sockets_duration_t result_ns;
      if (!sockets_method_tcp_socket_get_keep_alive_interval(
              socket_borrow, &result_ns, &error))
        return __wasilibc_socket_error_to_errno(error);

      uint64_t result_s = result_ns / NS_PER_S;
      if (result_s == 0) {
        result_s = 1; // Value was rounded down to zero. Round it up instead,
                      // because 0 is an invalid value for this socket option.
      }

      value = result_s > INT_MAX ? INT_MAX : result_s;
      break;
    }
    case TCP_KEEPCNT: {
      uint32_t result;
      if (!sockets_method_tcp_socket_get_keep_alive_count(socket_borrow,
                                                          &result, &error))
        return __wasilibc_socket_error_to_errno(error);

      value = result > INT_MAX ? INT_MAX : result;
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

static int tcp_setsockopt(void *data, int level, int optname,
                          const void *optval, socklen_t optlen) {
  tcp_socket_t *socket = (tcp_socket_t *)data;
  if (optlen < sizeof(int)) {
    errno = EINVAL;
    return -1;
  }
  int intval = *(int *)optval;

  sockets_error_code_t error;
  sockets_borrow_tcp_socket_t socket_borrow =
      sockets_borrow_tcp_socket(socket->socket);

  switch (level) {
  case SOL_SOCKET:
    switch (optname) {
    case SO_KEEPALIVE: {
      if (!sockets_method_tcp_socket_set_keep_alive_enabled(
              socket_borrow, intval != 0, &error))
        return __wasilibc_socket_error_to_errno(error);

      return 0;
    }
    case SO_RCVBUF: {
      if (!sockets_method_tcp_socket_set_receive_buffer_size(socket_borrow,
                                                             intval, &error))
        return __wasilibc_socket_error_to_errno(error);

      return 0;
    }
    case SO_SNDBUF: {
      if (!sockets_method_tcp_socket_set_send_buffer_size(socket_borrow, intval,
                                                          &error))
        return __wasilibc_socket_error_to_errno(error);

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
    case SO_RCVTIMEO: {
      if (optlen < sizeof(struct timeval)) {
        errno = EINVAL;
        return -1;
      }
      struct timeval *tv = (struct timeval *)optval;
      if (!timeval_to_duration(tv, &socket->recv_timeout)) {
        errno = EINVAL;
        return -1;
      }
      return 0;
    }
    case SO_SNDTIMEO: {
      if (optlen < sizeof(struct timeval)) {
        errno = EINVAL;
        return -1;
      }
      struct timeval *tv = (struct timeval *)optval;
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
      if (socket->family != SOCKETS_IP_ADDRESS_FAMILY_IPV4) {
        errno = EAFNOSUPPORT;
        return -1;
      }

      if (intval < 0 || intval > UINT8_MAX) {
        errno = EINVAL;
        return -1;
      }

      if (!sockets_method_tcp_socket_set_hop_limit(socket_borrow, intval,
                                                   &error))
        return __wasilibc_socket_error_to_errno(error);

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
      if (socket->family != SOCKETS_IP_ADDRESS_FAMILY_IPV6) {
        errno = EAFNOSUPPORT;
        return -1;
      }

      if (intval < 0 || intval > UINT8_MAX) {
        errno = EINVAL;
        return -1;
      }

      if (!sockets_method_tcp_socket_set_hop_limit(socket_borrow, intval,
                                                   &error))
        return __wasilibc_socket_error_to_errno(error);

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
      sockets_duration_t duration = intval * NS_PER_S;
      if (!sockets_method_tcp_socket_set_keep_alive_idle_time(socket_borrow,
                                                              duration, &error))
        return __wasilibc_socket_error_to_errno(error);

      return 0;
    }
    case TCP_KEEPINTVL: {
      sockets_duration_t duration = intval * NS_PER_S;
      if (!sockets_method_tcp_socket_set_keep_alive_interval(socket_borrow,
                                                             duration, &error))
        return __wasilibc_socket_error_to_errno(error);

      return 0;
    }
    case TCP_KEEPCNT: {
      if (!sockets_method_tcp_socket_set_keep_alive_count(socket_borrow, intval,
                                                          &error))
        return __wasilibc_socket_error_to_errno(error);

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

  // should not be reachable, all cases above should return
  abort();
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

#ifdef __wasip2__
    .poll_register = tcp_poll_register,
    .poll_finish = tcp_poll_finish,
#endif // __wasip2__

    .fcntl_getfl = tcp_fcntl_getfl,
    .fcntl_setfl = tcp_fcntl_setfl,
};

#endif // not(__wasip1__)
