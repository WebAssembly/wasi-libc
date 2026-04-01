#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <wasi/descriptor_table.h>
#include <wasi/sockets_utils.h>
#include <wasi/udp.h>
#include <wasi/wasip2.h>
#include <wasi/wasip3_block.h>

// Normalize on some wasip3 names
#ifdef __wasip2__
#define sockets_method_udp_socket_get_local_address                            \
  udp_method_udp_socket_local_address
#define sockets_method_udp_socket_get_remote_address                           \
  udp_method_udp_socket_remote_address
#define sockets_method_udp_socket_get_receive_buffer_size                      \
  udp_method_udp_socket_receive_buffer_size
#define sockets_method_udp_socket_get_send_buffer_size                         \
  udp_method_udp_socket_send_buffer_size
#define sockets_method_udp_socket_get_unicast_hop_limit                        \
  udp_method_udp_socket_unicast_hop_limit
#define sockets_method_udp_socket_set_receive_buffer_size                      \
  udp_method_udp_socket_set_receive_buffer_size
#define sockets_method_udp_socket_set_send_buffer_size                         \
  udp_method_udp_socket_set_send_buffer_size
#define sockets_method_udp_socket_set_unicast_hop_limit                        \
  udp_method_udp_socket_set_unicast_hop_limit
#endif

// Pollables here are lazily initialized as-needed.
typedef struct {
#if defined(__wasip2__)
  udp_own_incoming_datagram_stream_t incoming;
  poll_own_pollable_t incoming_pollable;
  udp_own_outgoing_datagram_stream_t outgoing;
  poll_own_pollable_t outgoing_pollable;
#elif defined(__wasip3__)
  /// Currently active call to `udp-socket#receive`.
  ///
  /// This subtask can be started both during `poll` or during a call to `recv`
  /// itself. If this is 0 then there's no active subtask.
  wasip3_subtask_t recv_subtask;

  /// Whether or not `recv_result` is valid.
  ///
  /// Note that this can be `true` while `recv_subtask` is 0, for example if
  /// `poll` receives an event which means `recv_subtask` is immediately closed
  /// but the result below is still valid.
  bool recv_ready;

  /// Storage space for the return value of `udp-socket#receive`.
  ///
  /// Only valid if `recv_ready` is `true`.
  sockets_result_tuple2_list_u8_ip_socket_address_error_code_t recv_result;

  /// Currently active call to `udp-socket#send`, if any.
  ///
  /// This is created in `sendto` and managed throughout a combination of that
  /// function and `poll`.
  ///
  /// If this field is 0 then there's no active subtask.
  wasip3_subtask_t send_subtask;

  /// The on-heap arguments to `udp-socket#send`. Used whenever `send` is
  /// called.
  sockets_method_udp_socket_send_args_t send_args;

  /// Whether or not `send_result` is ready to read.
  ///
  /// Similar to `recv_ready` this can be `true` while `send_subtask` is 0.
  bool send_result_ready;

  /// On-heap result of `udp-socket#send`, only readable if `send_result_ready`
  /// is `true`.
  sockets_result_void_error_code_t send_result;
#else
#error "Unsupported WASI version"
#endif
} udp_socket_streams_t;

typedef struct {
  int dummy;
} udp_socket_state_unbound_t;
typedef struct {
  int dummy;
} udp_socket_state_bound_nostreams_t;

typedef struct {
  udp_socket_streams_t streams; // Streams have no remote_address
} udp_socket_state_bound_streaming_t;

typedef struct {
  udp_socket_streams_t streams; // Streams have a remote_address
} udp_socket_state_connected_t;

// This is a tagged union. When adding/removing/renaming cases, be sure to keep
// the tag and union definitions in sync. The "bound" state is split up into two
// distinct tags:
// - "bound_nostreams": Bound, but no datagram streams set up (yet). That will
// be done the first time send or recv is called.
// - "bound_streaming": Bound with active streams.
typedef struct {
  enum {
    UDP_SOCKET_STATE_UNBOUND,
    UDP_SOCKET_STATE_BOUND_NOSTREAMS,
    UDP_SOCKET_STATE_BOUND_STREAMING,
    UDP_SOCKET_STATE_CONNECTED,
  } tag;
  union {
    udp_socket_state_unbound_t unbound;
    udp_socket_state_bound_nostreams_t bound_nostreams;
    udp_socket_state_bound_streaming_t bound_streaming;
    udp_socket_state_connected_t connected;
  };
} udp_socket_state_t;

typedef struct {
  sockets_own_udp_socket_t socket;
#ifdef __wasip2__
  // Lazily initialized as-needed pollable.
  poll_own_pollable_t socket_pollable;
#endif
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

static int udp_create_streams(udp_socket_t *socket,
                              sockets_ip_socket_address_t *remote_address) {
  // Assert that:
  // - We're already bound. This is required by WASI.
  // - We have no active streams. From WASI:
  //   > Implementations may trap if the streams returned by a previous
  //   > invocation haven't been dropped yet before calling `stream` again.
  if (socket->state.tag != UDP_SOCKET_STATE_BOUND_NOSTREAMS) {
    abort();
  }

  udp_socket_streams_t *dst;
  if (remote_address != NULL) {
    dst = &socket->state.connected.streams;
  } else {
    dst = &socket->state.bound_streaming.streams;
  }

  memset(dst, 0, sizeof(*dst));

  sockets_borrow_udp_socket_t socket_borrow =
      sockets_borrow_udp_socket(socket->socket);
  sockets_error_code_t error;
#ifdef __wasip2__
  udp_tuple2_own_incoming_datagram_stream_own_outgoing_datagram_stream_t io;
  if (!udp_method_udp_socket_stream(socket_borrow, remote_address, &io, &error))
    return __wasilibc_socket_error_to_errno(&error);

  dst->incoming = io.f0;
  dst->outgoing = io.f1;
#else
  if (remote_address &&
      !sockets_method_udp_socket_connect(socket_borrow, remote_address, &error))
    return __wasilibc_socket_error_to_errno(&error);
#endif

  if (remote_address != NULL) {
    socket->state.tag = UDP_SOCKET_STATE_CONNECTED;
  } else {
    socket->state.tag = UDP_SOCKET_STATE_BOUND_STREAMING;
  }

  return 0;
}

static void udp_close_streams(udp_socket_streams_t *streams) {
#ifdef __wasip2__
  if (streams->incoming_pollable.__handle != 0) {
    poll_pollable_drop_own(streams->incoming_pollable);
    streams->incoming_pollable.__handle = 0;
  }
  udp_incoming_datagram_stream_drop_own(streams->incoming);
  streams->incoming.__handle = 0;
  if (streams->outgoing_pollable.__handle != 0) {
    poll_pollable_drop_own(streams->outgoing_pollable);
    streams->outgoing_pollable.__handle = 0;
  }
  udp_outgoing_datagram_stream_drop_own(streams->outgoing);
  streams->outgoing.__handle = 0;
#else
  // If there's an active `recv` then cancel it since nothing's ever going to
  // receive the result. This conditionally flags the result as valid depending
  // on the result of cancellation.
  if (streams->recv_subtask) {
    assert(!streams->recv_ready);
    wasip3_subtask_status_t status =
        wasip3_subtask_cancel(streams->recv_subtask);
    wasip3_subtask_drop(streams->recv_subtask);
    streams->recv_subtask = 0;
    if (WASIP3_SUBTASK_STATE(status) == WASIP3_SUBTASK_RETURNED)
      streams->recv_ready = true;
  }

  // If a packet is buffered within this socket, then deallocate it and flag it
  // as no longer valid.
  if (streams->recv_ready) {
    sockets_result_tuple2_list_u8_ip_socket_address_error_code_free(
        &streams->recv_result);
    streams->recv_ready = false;
  }

  // Unlike for `recv_subtask` if there's an active packet being sent this
  // blocks and waits for the result. Note that this means that `close` is a
  // blocking operation, even if the socket is in nonblocking mode. There's some
  // more discussion in WebAssembly/component-model#617. Currently it's at least
  // intentional that this blocks on the result.
  if (streams->send_subtask) {
    assert(!streams->send_result_ready);
    __wasilibc_subtask_block_on_and_drop(streams->send_subtask);
    streams->send_subtask = 0;
    streams->send_result_ready = true;
  }

  // If there's an active buffer in `send_args` then that's allocated on the
  // heap, so deallocate it here to clean up its memory.
  if (streams->send_args.data.ptr) {
    free(streams->send_args.data.ptr);
    streams->send_args.data.ptr = NULL;
    streams->send_args.data.len = 0;
  }
  streams->send_result_ready = false;
#endif
}

/// Returns the internal `udp_socket_streams_t*` in use, or `NULL` if the socket
/// isn't in such a state.
static udp_socket_streams_t *udp_streams(udp_socket_t *socket) {
  switch (socket->state.tag) {
  case UDP_SOCKET_STATE_BOUND_STREAMING:
    return &socket->state.bound_streaming.streams;
  case UDP_SOCKET_STATE_CONNECTED:
    return &socket->state.connected.streams;
  default:
    return NULL;
  }
}

static void udp_free(void *data) {
  udp_socket_t *udp = (udp_socket_t *)data;

  udp_socket_streams_t *streams = udp_streams(udp);
  if (streams)
    udp_close_streams(streams);

#ifdef __wasip2__
  if (udp->socket_pollable.__handle != 0)
    poll_pollable_drop_own(udp->socket_pollable);
#endif
  sockets_udp_socket_drop_own(udp->socket);

  free(udp);
}

static int udp_set_blocking(void *data, bool blocking) {
  udp_socket_t *udp = (udp_socket_t *)data;
  udp->blocking = blocking;
  return 0;
}

static int udp_fstat(void *data, struct stat *buf) {
  (void)data;
  memset(buf, 0, sizeof(struct stat));
  buf->st_mode = S_IFSOCK;
  return 0;
}

#ifdef __wasip2__
static poll_borrow_pollable_t udp_pollable(udp_socket_t *socket) {
  if (socket->socket_pollable.__handle == 0) {
    udp_borrow_udp_socket_t socket_borrow =
        udp_borrow_udp_socket(socket->socket);
    socket->socket_pollable = udp_method_udp_socket_subscribe(socket_borrow);
  }
  return poll_borrow_pollable(socket->socket_pollable);
}

static poll_borrow_pollable_t
udp_incoming_pollable(udp_socket_streams_t *streams) {
  if (streams->incoming_pollable.__handle == 0) {
    streams->incoming_pollable = udp_method_incoming_datagram_stream_subscribe(
        udp_borrow_incoming_datagram_stream(streams->incoming));
  }
  return poll_borrow_pollable(streams->incoming_pollable);
}

static poll_borrow_pollable_t
udp_outgoing_pollable(udp_socket_streams_t *streams) {
  if (streams->outgoing_pollable.__handle == 0) {
    streams->outgoing_pollable = udp_method_outgoing_datagram_stream_subscribe(
        udp_borrow_outgoing_datagram_stream(streams->outgoing));
  }
  return poll_borrow_pollable(streams->outgoing_pollable);
}

static int udp_handle_error(udp_socket_t *socket, sockets_error_code_t *error) {
  if (*error == NETWORK_ERROR_CODE_WOULD_BLOCK && socket->blocking) {
    poll_method_pollable_block(udp_pollable(socket));
  } else {
    return __wasilibc_socket_error_to_errno(error);
  }

  return 0;
}
#endif

static int udp_do_bind(udp_socket_t *socket,
                       sockets_ip_socket_address_t *address) {
  if (socket->state.tag != UDP_SOCKET_STATE_UNBOUND) {
    errno = EINVAL;
    return -1;
  }

  sockets_error_code_t error;
#ifdef __wasip2__
  network_borrow_network_t network_borrow =
      __wasi_sockets_utils__borrow_network();
  udp_borrow_udp_socket_t socket_borrow = udp_borrow_udp_socket(socket->socket);

  if (!udp_method_udp_socket_start_bind(socket_borrow, network_borrow, address,
                                        &error))
    return __wasilibc_socket_error_to_errno(&error);

  // Bind has successfully started. Attempt to finish it:
  while (!udp_method_udp_socket_finish_bind(socket_borrow, &error)) {
    if (udp_handle_error(socket, &error) < 0)
      return -1;
  }
#else
  sockets_borrow_udp_socket_t socket_borrow =
      sockets_borrow_udp_socket(socket->socket);
  if (!sockets_method_udp_socket_bind(socket_borrow, address, &error))
    return __wasilibc_socket_error_to_errno(&error);

#endif

  // Bind successful.
  socket->state.tag = UDP_SOCKET_STATE_BOUND_NOSTREAMS;
  return 0;
}

static int udp_bind(void *data, const struct sockaddr *addr,
                    socklen_t addrlen) {
  udp_socket_t *socket = (udp_socket_t *)data;
  sockets_ip_socket_address_t local_address;
  if (__wasilibc_sockaddr_to_wasi(socket->family, addr, addrlen,
                                  &local_address) < 0)
    return -1;
  return udp_do_bind(socket, &local_address);
}

// When `connect` is called on a UDP socket with an AF_UNSPEC address, it is
// actually a "disconnect" request.
static int udp_connect(void *data, const struct sockaddr *addr,
                       socklen_t addrlen) {
  udp_socket_t *socket = (udp_socket_t *)data;
  if (addr == NULL || addrlen < sizeof(struct sockaddr)) {
    errno = EINVAL;
    return -1;
  }

  sockets_ip_socket_address_t remote_address;
  bool has_remote_address = (addr->sa_family != AF_UNSPEC);
  if (has_remote_address &&
      __wasilibc_sockaddr_to_wasi(socket->family, addr, addrlen,
                                  &remote_address) < 0)
    return -1;

  // Prepare the socket; binding it if not bound yet, and disconnecting it if
  // connected.
  switch (socket->state.tag) {
  case UDP_SOCKET_STATE_UNBOUND: {
    // Socket is not explicitly bound by the user. We'll do it for them:
    sockets_ip_socket_address_t any;
    __wasilibc_unspecified_addr(socket->family, &any);
    if (udp_do_bind(socket, &any) < 0)
      return -1;
    break;
  }
  case UDP_SOCKET_STATE_BOUND_NOSTREAMS: {
    // This is the state we want to be in.
    break;
  }
  case UDP_SOCKET_STATE_BOUND_STREAMING: {
    udp_close_streams(&socket->state.bound_streaming.streams);
    socket->state.tag = UDP_SOCKET_STATE_BOUND_NOSTREAMS;
    break;
  }
  case UDP_SOCKET_STATE_CONNECTED: {
    udp_close_streams(&socket->state.connected.streams);
    socket->state.tag = UDP_SOCKET_STATE_BOUND_NOSTREAMS;
    break;
  }
  default:
    /* unreachable */
    abort();
  }

  return udp_create_streams(socket,
                            has_remote_address ? &remote_address : NULL);
}

static int udp_getsockname(void *data, struct sockaddr *addr,
                           socklen_t *addrlen) {
  udp_socket_t *socket = (udp_socket_t *)data;
  output_sockaddr_t output_addr;
  if (__wasilibc_sockaddr_validate(socket->family, addr, addrlen,
                                   &output_addr) < 0)
    return -1;

  if (output_addr.tag == OUTPUT_SOCKADDR_NULL) {
    errno = EINVAL;
    return -1;
  }

  switch (socket->state.tag) {
  case UDP_SOCKET_STATE_UNBOUND:
    errno = EINVAL;
    return -1;

  case UDP_SOCKET_STATE_BOUND_NOSTREAMS:
  case UDP_SOCKET_STATE_BOUND_STREAMING:
  case UDP_SOCKET_STATE_CONNECTED:
    // OK. Continue..
    break;

  default: /* unreachable */
    abort();
  }

  sockets_error_code_t error;
  sockets_ip_socket_address_t result;
  sockets_borrow_udp_socket_t socket_borrow =
      sockets_borrow_udp_socket(socket->socket);
  if (!sockets_method_udp_socket_get_local_address(socket_borrow, &result,
                                                   &error))
    return __wasilibc_socket_error_to_errno(&error);

  __wasilibc_wasi_to_sockaddr(result, &output_addr);
  return 0;
}

static int udp_getpeername(void *data, struct sockaddr *addr,
                           socklen_t *addrlen) {
  udp_socket_t *socket = (udp_socket_t *)data;
  output_sockaddr_t output_addr;
  if (__wasilibc_sockaddr_validate(socket->family, addr, addrlen,
                                   &output_addr) < 0)
    return -1;

  if (output_addr.tag == OUTPUT_SOCKADDR_NULL) {
    errno = EINVAL;
    return -1;
  }

  switch (socket->state.tag) {
  case UDP_SOCKET_STATE_UNBOUND:
  case UDP_SOCKET_STATE_BOUND_NOSTREAMS:
  case UDP_SOCKET_STATE_BOUND_STREAMING:
    errno = ENOTCONN;
    return -1;

  case UDP_SOCKET_STATE_CONNECTED:
    // OK. Continue..
    break;

  default: /* unreachable */
    abort();
  }

  sockets_error_code_t error;
  sockets_ip_socket_address_t result;
  sockets_borrow_udp_socket_t socket_borrow =
      sockets_borrow_udp_socket(socket->socket);
  if (!sockets_method_udp_socket_get_remote_address(socket_borrow, &result,
                                                    &error))
    return __wasilibc_socket_error_to_errno(&error);

  __wasilibc_wasi_to_sockaddr(result, &output_addr);
  return 0;
}

#ifdef __wasip3__

// Starts the process of receiving packet on `socket`.
//
// This will kick off a call to `udp-socket#receive` to store the result in
// `streams`. Returns `true` if a subtask was started here, and returns `false`
// if the subtask was already in progress or a packet is already ready.
static bool wasip3_recv_start(udp_socket_t *socket,
                              udp_socket_streams_t *streams) {
  if (streams->recv_subtask || streams->recv_ready)
    return false;
  wasip3_subtask_status_t status = sockets_method_udp_socket_receive(
      sockets_borrow_udp_socket(socket->socket), &streams->recv_result);
  if (WASIP3_SUBTASK_STATE(status) == WASIP3_SUBTASK_RETURNED) {
    streams->recv_ready = true;
  } else {
    streams->recv_subtask = WASIP3_SUBTASK_HANDLE(status);
  }
  return true;
}

// Ends the process of receiving a packet on `socket`.
//
// Processes `event` from whence it came and updates internal state.
static void wasip3_recv_end(udp_socket_streams_t *streams,
                            wasip3_event_t *event) {
  (void)event;
  assert(event->waitable == streams->recv_subtask);
  assert(event->code == WASIP3_SUBTASK_RETURNED);
  wasip3_subtask_drop(streams->recv_subtask);
  streams->recv_subtask = 0;
  streams->recv_ready = true;
}

static void wasip3_recv_ready(void *data, poll_state_t *state,
                              wasip3_event_t *event) {
  udp_socket_t *socket = (udp_socket_t *)data;
  udp_socket_streams_t *streams = udp_streams(socket);
  assert(streams);

  wasip3_recv_end(streams, event);
  __wasilibc_poll_ready(state, POLLRDNORM);
}

#endif // __wasip3__

static ssize_t udp_recvfrom(void *data, void *buffer, size_t length, int flags,
                            struct sockaddr *addr, socklen_t *addrlen) {
  udp_socket_t *socket = (udp_socket_t *)data;
  // TODO wasi-sockets: flags:
  // - MSG_PEEK: could be shimmed by performing the receive into a local
  // socket-specific buffer. And on subsequent receives first check that buffer.

  const int supported_flags = MSG_DONTWAIT | MSG_TRUNC;
  if ((flags & supported_flags) != flags) {
    errno = EOPNOTSUPP;
    return -1;
  }

  output_sockaddr_t output_addr;
  if (__wasilibc_sockaddr_validate(socket->family, addr, addrlen,
                                   &output_addr) < 0)
    return -1;

  // Make sure the streams are available.
  if (socket->state.tag == UDP_SOCKET_STATE_BOUND_NOSTREAMS)
    if (udp_create_streams(socket, NULL) < 0)
      return -1;

  udp_socket_streams_t *streams = udp_streams(socket);
  if (!streams) {
    // Unlike `send`, `recv` should _not_ perform an implicit bind.
    errno = EINVAL;
    return -1;
  }

  bool return_real_size = (flags & MSG_TRUNC) != 0;
  bool should_block = socket->blocking;
  if ((flags & MSG_DONTWAIT) != 0) {
    should_block = false;
  }

#ifdef __wasip2__
  sockets_error_code_t error;
  udp_borrow_incoming_datagram_stream_t incoming_borrow =
      udp_borrow_incoming_datagram_stream(streams->incoming);
  while (true) {
    udp_list_incoming_datagram_t datagrams;
    if (!udp_method_incoming_datagram_stream_receive(incoming_borrow, 1,
                                                     &datagrams, &error))
      return __wasilibc_socket_error_to_errno(&error);

    if (datagrams.len) {
      udp_incoming_datagram_t datagram = datagrams.ptr[0];
      size_t datagram_size = datagram.data.len;
      size_t bytes_to_copy = datagram_size < length ? datagram_size : length;

      if (output_addr.tag != OUTPUT_SOCKADDR_NULL)
        __wasilibc_wasi_to_sockaddr(datagram.remote_address, &output_addr);

      memcpy(buffer, datagram.data.ptr, bytes_to_copy);
      udp_list_incoming_datagram_free(&datagrams);
      return return_real_size ? datagram_size : bytes_to_copy;
    }
    if (!should_block) {
      errno = EWOULDBLOCK;
      return -1;
    }

    poll_method_pollable_block(udp_incoming_pollable(streams));
  }
#else
  // Ensure there's a subtask started or a packet ready. If this is a
  // nonblocking operation and a subtask was started then polling below isn't
  // very useful, so go ahead and return `EWOULDBLOCK`.
  bool started = wasip3_recv_start(socket, streams);
  if (started && !should_block) {
    errno = EWOULDBLOCK;
    return -1;
  }

  // If there's an active subtask, then block on its resolution if that's
  // desired. For nonblocking receives check on the result that was already in
  // flight.
  if (streams->recv_subtask) {
    if (should_block) {
      __wasilibc_subtask_block_on_and_drop(streams->recv_subtask);
      streams->recv_subtask = 0;
      streams->recv_ready = true;
    } else {
      wasip3_event_t event;
      __wasilibc_poll_waitable(streams->recv_subtask, &event);
      if (event.event == WASIP3_EVENT_SUBTASK) {
        wasip3_recv_end(streams, &event);
      } else {
        assert(event.event == WASIP3_EVENT_NONE);
        errno = EWOULDBLOCK;
        return -1;
      }
    }
  }

  // At this point we're finishing up an I/O operation. This involves copying
  // out the result to the caller. Reset the `recv_ready` flag and then
  // process the result that's in the `streams` storage.
  assert(streams->recv_ready);
  assert(streams->recv_subtask == 0);
  streams->recv_ready = false;

  // If the operation failed, just translate its error and return.
  if (streams->recv_result.is_err)
    return __wasilibc_socket_error_to_errno(&streams->recv_result.val.err);

  // Fill in the source address if requested.
  if (output_addr.tag != OUTPUT_SOCKADDR_NULL)
    __wasilibc_wasi_to_sockaddr(streams->recv_result.val.ok.f1, &output_addr);

  // Copy out the datagram to the caller.
  size_t datagram_size = streams->recv_result.val.ok.f0.len;
  size_t bytes_to_copy = datagram_size < length ? datagram_size : length;
  memcpy(buffer, streams->recv_result.val.ok.f0.ptr, bytes_to_copy);
  wasip3_list_u8_free(&streams->recv_result.val.ok.f0);
  return return_real_size ? datagram_size : bytes_to_copy;
#endif
}

#ifdef __wasip3__

static int wasip3_send_resolve(udp_socket_streams_t *streams,
                               bool should_block) {
  // If a previous packet is in flight then wait for that.
  //
  // * If this is a blocking context, then block here on the previous subtask.
  // * If this is a nonblocking context, poll the result and see what happens,
  //   bailing out if the subtask isn't ready yet.
  //
  // Upon completion of the subtask it's cleared out and then the result is
  // flagged as ready for processing below. Note that there may not be a
  // subtask but the result may be ready for processing (e.g. a previous call
  // to `poll`), so the result processing is split out.
  if (streams->send_subtask) {
    if (should_block) {
      __wasilibc_subtask_block_on_and_drop(streams->send_subtask);
      streams->send_subtask = 0;
    } else {
      wasip3_event_t event;
      __wasilibc_poll_waitable(streams->send_subtask, &event);
      if (event.event == WASIP3_EVENT_SUBTASK) {
        // fall through ...
      } else {
        assert(event.event == WASIP3_EVENT_NONE);
        errno = EWOULDBLOCK;
        return -1;
      }
    }
    streams->send_result_ready = true;
    if (streams->send_subtask) {
      wasip3_subtask_drop(streams->send_subtask);
      streams->send_subtask = 0;
    }
  }

  // Check to see what happened with the previous send if the result is ready.
  // This is technically deferring an error on a call to `send` to the next call
  // to `send` for nonblocking I/O, but that's normal for UDP anyway.
  if (streams->send_result_ready) {
    free(streams->send_args.data.ptr);
    streams->send_args.data.ptr = NULL;
    streams->send_args.data.len = 0;
    streams->send_result_ready = false;
    if (streams->send_result.is_err)
      return __wasilibc_socket_error_to_errno(&streams->send_result.val.err);
  }

  return 0;
}

static void wasip3_send_ready(void *data, poll_state_t *state,
                              wasip3_event_t *event) {
  (void)event;

  udp_socket_t *socket = (udp_socket_t *)data;
  udp_socket_streams_t *streams = udp_streams(socket);
  assert(streams);

  assert(event->waitable == streams->send_subtask);
  assert(event->code == WASIP3_SUBTASK_RETURNED);
  assert(!streams->send_result_ready);
  wasip3_subtask_drop(streams->send_subtask);
  streams->send_subtask = 0;
  streams->send_result_ready = true;

  __wasilibc_poll_ready(state, POLLWRNORM);
}

#endif // __wasip3__

static ssize_t udp_sendto(void *data, const void *buffer, size_t length,
                          int flags, const struct sockaddr *addr,
                          socklen_t addrlen) {
  udp_socket_t *socket = (udp_socket_t *)data;
  const int supported_flags = MSG_DONTWAIT;
  if ((flags & supported_flags) != flags) {
    errno = EOPNOTSUPP;
    return -1;
  }

  sockets_ip_socket_address_t remote_address;
  bool has_remote_address = (addr != NULL);

  if (has_remote_address) {
    if (socket->state.tag == UDP_SOCKET_STATE_CONNECTED) {
      errno = EISCONN;
      return -1;
    }

    if (__wasilibc_sockaddr_to_wasi(socket->family, addr, addrlen,
                                    &remote_address) < 0)
      return -1;
  } else {
    if (addrlen != 0) {
      errno = EINVAL;
      return -1;
    }

    if (socket->state.tag != UDP_SOCKET_STATE_CONNECTED) {
      errno = EDESTADDRREQ;
      return -1;
    }
  }

  // If the socket is not explicitly bound by the user we'll do it for them
  if (socket->state.tag == UDP_SOCKET_STATE_UNBOUND) {
    sockets_ip_socket_address_t any;
    __wasilibc_unspecified_addr(socket->family, &any);
    if (udp_do_bind(socket, &any) < 0)
      return -1;
  }

  // Make sure the streams are available.
  if (socket->state.tag == UDP_SOCKET_STATE_BOUND_NOSTREAMS)
    if (udp_create_streams(socket, NULL) < 0)
      return -1;

  udp_socket_streams_t *streams = udp_streams(socket);
  ;
  if (!streams) {
    errno = EINVAL;
    return -1;
  }

  bool should_block = socket->blocking;
  if ((flags & MSG_DONTWAIT) != 0) {
    should_block = false;
  }

#ifdef __wasip2__
  udp_outgoing_datagram_t datagrams[1] = {{
      .remote_address =
          {
              .is_some = has_remote_address,
              .val = remote_address,
          },
      .data =
          {
              .len = length,
              .ptr = (uint8_t *)buffer,
          },
  }};
  udp_list_outgoing_datagram_t list = {
      .len = 1,
      .ptr = datagrams,
  };

  sockets_error_code_t error;
  udp_borrow_outgoing_datagram_stream_t outgoing_borrow =
      udp_borrow_outgoing_datagram_stream(streams->outgoing);
  while (true) {
    uint64_t allowed;
    if (!udp_method_outgoing_datagram_stream_check_send(outgoing_borrow,
                                                        &allowed, &error))
      return __wasilibc_socket_error_to_errno(&error);

    if (allowed) {
      uint64_t datagrams_sent;
      if (!udp_method_outgoing_datagram_stream_send(outgoing_borrow, &list,
                                                    &datagrams_sent, &error))
        return __wasilibc_socket_error_to_errno(&error);

      if (datagrams_sent != 0 && datagrams_sent != 1) {
        abort();
      }

      if (datagrams_sent == 1) {
        return length;
      }
    }

    if (!should_block) {
      errno = EWOULDBLOCK;
      return -1;
    }
    poll_method_pollable_block(udp_outgoing_pollable(streams));
  }
#else
  // There might be a previous subtask or result waiting for us from a previous
  // subtask. If that's the case pick up the result here and transfer the result
  // to the application. This means things get a bit weird where errors from
  // a previous `send` get reported on the next `send` (or never if the
  // application never calls `send` again), but it's not clear if libc can
  // do better with the current wasip3 APIs.
  if (wasip3_send_resolve(streams, should_block) < 0)
    return -1;

  // At this point there's no active send, so go ahead and issue another send.
  // First try with the caller's provided buffer and see what happens, but we
  // can't borrow the caller's buffer indefinitely so if that fails then we
  // have to cancel the subtask, copy to internal storage, and then try again.
  assert(!streams->send_subtask);
  assert(!streams->send_result_ready);
  assert(!streams->send_args.data.ptr);

  streams->send_args.self = sockets_borrow_udp_socket(socket->socket);
  streams->send_args.data.ptr = (uint8_t *)buffer;
  streams->send_args.data.len = length;
  streams->send_args.remote_address.is_some = has_remote_address;
  if (has_remote_address)
    streams->send_args.remote_address.val = remote_address;
  wasip3_subtask_status_t status = sockets_method_udp_socket_send(
      &streams->send_args, &streams->send_result);

  // The subtask didn't immediately return, so cancel it. Note that while
  // cancelling the subtask might return, so that's handled by falling through.
  if (WASIP3_SUBTASK_STATE(status) != WASIP3_SUBTASK_RETURNED) {
    wasip3_subtask_t subtask = WASIP3_SUBTASK_HANDLE(status);
    status = wasip3_subtask_cancel(subtask);
    wasip3_subtask_drop(subtask);
  }

  // If the subtask returned, or if it was in-progress and cancelled, either way
  // the arguments are no longer in use. Revoke the user-supplied buffer from
  // the `streams` state to ensure it's not accidentally free'd later on.
  streams->send_args.data.ptr = NULL;
  streams->send_args.data.len = 0;

  // If the task returned early, we'll skip the above block and this block.
  // Otherwise if the task didn't return it executed the above block and updated
  // `status`, so that's then re-evaluated here. If the cancellation operation
  // didn't end up actually completing anything then re-issue another send
  // operation after copying the packet into the socket's internal storage.
  if (WASIP3_SUBTASK_STATE(status) != WASIP3_SUBTASK_RETURNED) {
    streams->send_args.data.ptr = malloc(length);
    if (!streams->send_args.data.ptr) {
      errno = ENOMEM;
      return -1;
    }
    memcpy(streams->send_args.data.ptr, buffer, length);
    streams->send_args.data.len = length;
    status = sockets_method_udp_socket_send(&streams->send_args,
                                            &streams->send_result);
  }

  if (WASIP3_SUBTASK_STATE(status) != WASIP3_SUBTASK_RETURNED) {
    streams->send_subtask = WASIP3_SUBTASK_HANDLE(status);
  } else {
    assert(streams->send_subtask == 0);
    assert(!streams->send_result_ready);
    streams->send_result_ready = true;
  }

  // If blocking I/O is enabled, wait for the result. Also go ahead and resolve
  // things if the result is ready from an async perspective. If this is
  // nonblocking, however, and the stream result isn't present, then consider
  // this as good to go in the background and return that the packet is sent.
  if (should_block || streams->send_result_ready) {
    if (wasip3_send_resolve(streams, should_block) < 0)
      return -1;
  }

  return length;
#endif
}

static int udp_getsockopt(void *data, int level, int optname, void *optval,
                          socklen_t *optlen) {
  udp_socket_t *socket = (udp_socket_t *)data;
  int value;

  sockets_error_code_t error;
  sockets_borrow_udp_socket_t socket_borrow =
      sockets_borrow_udp_socket(socket->socket);

  switch (level) {
  case SOL_SOCKET:
    switch (optname) {
    case SO_TYPE: {
      value = SOCK_DGRAM;
      break;
    }
    case SO_PROTOCOL: {
      value = IPPROTO_UDP;
      break;
    }
    case SO_DOMAIN: {
      value = __wasilibc_wasi_family_to_libc(socket->family);
      break;
    }

    case SO_ERROR:
      value = 0;
      break;

    case SO_RCVBUF: {
      uint64_t result;
      if (!sockets_method_udp_socket_get_receive_buffer_size(socket_borrow,
                                                             &result, &error))
        return __wasilibc_socket_error_to_errno(&error);

      if (result > INT_MAX) {
        abort();
      }

      value = result;
      break;
    }
    case SO_SNDBUF: {
      uint64_t result;
      if (!sockets_method_udp_socket_get_send_buffer_size(socket_borrow,
                                                          &result, &error))
        return __wasilibc_socket_error_to_errno(&error);

      if (result > INT_MAX) {
        abort();
      }

      value = result;
      break;
    }
    case SO_RCVTIMEO: // TODO wasi-sockets: emulate in wasi-libc itself
    case SO_SNDTIMEO: // TODO wasi-sockets: emulate in wasi-libc itself
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
      if (!sockets_method_udp_socket_get_unicast_hop_limit(socket_borrow,
                                                           &result, &error))
        return __wasilibc_socket_error_to_errno(&error);

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
      if (!sockets_method_udp_socket_get_unicast_hop_limit(socket_borrow,
                                                           &result, &error))
        return __wasilibc_socket_error_to_errno(&error);

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

static int udp_setsockopt(void *data, int level, int optname,
                          const void *optval, socklen_t optlen) {
  udp_socket_t *socket = (udp_socket_t *)data;
  if (optlen < sizeof(int)) {
    errno = EINVAL;
    return -1;
  }
  int intval = *(int *)optval;

  sockets_error_code_t error;
  sockets_borrow_udp_socket_t socket_borrow =
      sockets_borrow_udp_socket(socket->socket);

  switch (level) {
  case SOL_SOCKET:
    switch (optname) {
    case SO_RCVBUF: {
      if (!sockets_method_udp_socket_set_receive_buffer_size(socket_borrow,
                                                             intval, &error))
        return __wasilibc_socket_error_to_errno(&error);

      return 0;
    }
    case SO_SNDBUF: {
      if (!sockets_method_udp_socket_set_send_buffer_size(socket_borrow, intval,
                                                          &error))
        return __wasilibc_socket_error_to_errno(&error);

      return 0;
    }
    case SO_RCVTIMEO: // TODO wasi-sockets: emulate in wasi-libc itself
    case SO_SNDTIMEO: // TODO wasi-sockets: emulate in wasi-libc itself
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

      if (!sockets_method_udp_socket_set_unicast_hop_limit(socket_borrow,
                                                           intval, &error))
        return __wasilibc_socket_error_to_errno(&error);

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

      if (!sockets_method_udp_socket_set_unicast_hop_limit(socket_borrow,
                                                           intval, &error))
        return __wasilibc_socket_error_to_errno(&error);

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

static int udp_poll_register(void *data, poll_state_t *state, short events) {
  udp_socket_t *socket = (udp_socket_t *)data;
  switch (socket->state.tag) {
  case UDP_SOCKET_STATE_UNBOUND:
    // In the unbound case, all we can do is assume immediate read and write
    // readiness:
    __wasilibc_poll_ready(state, events);
    break;

  case UDP_SOCKET_STATE_BOUND_NOSTREAMS:
    // We can't know the true state of read and write readiness until we have
    // stream handles, so let's add those now:
    if (udp_create_streams(socket, NULL) < 0)
      return -1;
    // Deliberately fall through to next case:

  case UDP_SOCKET_STATE_BOUND_STREAMING:
  case UDP_SOCKET_STATE_CONNECTED: {
    udp_socket_streams_t *streams = udp_streams(socket);
    assert(streams);
#ifdef __wasip2__
    if ((events & POLLRDNORM) != 0) {
      if (__wasilibc_poll_add(state, POLLRDNORM,
                              udp_incoming_pollable(streams)) < 0)
        return -1;
    }
    if ((events & POLLWRNORM) != 0) {
      if (__wasilibc_poll_add(state, POLLWRNORM,
                              udp_outgoing_pollable(streams)) < 0)
        return -1;
    }
#else
    // If readability of this socket is desired, there's a few parts of the
    // state machine here:
    //
    // 1. First kick off a receive of a packet if there's not already one
    //    pending.
    // 2. If there's a packet waiting already buffered in this socket, then
    //    it's declared immediately ready.
    // 3. Otherwise this means that a receive is in-flight. In this situation
    //    we're waiting on the subtask so it's added to the set of things to
    //    poll.
    if ((events & POLLRDNORM) != 0) {
      wasip3_recv_start(socket, streams);
      if (streams->recv_ready) {
        __wasilibc_poll_ready(state, POLLRDNORM);
      } else {
        assert(streams->recv_subtask != 0);
        if (__wasilibc_poll_add(state, streams->recv_subtask, wasip3_recv_ready,
                                data) < 0)
          return -1;
      }
    }

    // Writability is a bit of an inversion of readability. If there's no
    // active write in progress, then this is immediately ready to write.
    // Otherwise we're waiting on the write subtask.
    if ((events & POLLWRNORM) != 0) {
      if (streams->send_subtask) {
        if (__wasilibc_poll_add(state, streams->send_subtask, wasip3_send_ready,
                                data) < 0)
          return -1;
      } else {
        __wasilibc_poll_ready(state, POLLWRNORM);
      }
    }
#endif
    break;
  }
  default:
    errno = ENOTSUP;
    return -1;
  }
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
    .connect = udp_connect,
    .getsockname = udp_getsockname,
    .getpeername = udp_getpeername,
    .recvfrom = udp_recvfrom,
    .sendto = udp_sendto,
    .getsockopt = udp_getsockopt,
    .setsockopt = udp_setsockopt,
    .poll_register = udp_poll_register,
    .fcntl_getfl = udp_fcntl_getfl,
    .fcntl_setfl = udp_fcntl_setfl,
};
