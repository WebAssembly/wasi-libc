#ifdef __wasilibc_use_wasip2

#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <wasi/descriptor_table.h>
#include <wasi/sockets_utils.h>
#include <wasi/udp.h>
#include <wasi/wasip2.h>

// Pollables here are lazily initialized as-needed.
typedef struct {
  udp_own_incoming_datagram_stream_t incoming;
  poll_own_pollable_t incoming_pollable;
  udp_own_outgoing_datagram_stream_t outgoing;
  poll_own_pollable_t outgoing_pollable;
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

// This is a tagged union. When adding/removing/renaming cases, be sure to keep the tag and union definitions in sync.
// The "bound" state is split up into two distinct tags:
// - "bound_nostreams": Bound, but no datagram streams set up (yet). That will be done the first time send or recv is called.
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
  udp_own_udp_socket_t socket;
  // Lazily initialized as-needed pollable.
  poll_own_pollable_t socket_pollable;
  bool blocking;
  network_ip_address_family_t family;
  udp_socket_state_t state;
} udp_socket_t;

static descriptor_vtable_t udp_vtable;

int __wasilibc_add_udp_socket(udp_own_udp_socket_t socket,
                              network_ip_address_family_t family,
                              bool blocking) {
  udp_socket_t *udp = calloc(1, sizeof(udp_socket_t));
  if (!udp) {
    udp_udp_socket_drop_own(socket);
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
                              network_ip_socket_address_t *remote_address) {
  // Assert that:
  // - We're already bound. This is required by WASI.
  // - We have no active streams. From WASI:
  //   > Implementations may trap if the streams returned by a previous
  //   > invocation haven't been dropped yet before calling `stream` again.
  if (socket->state.tag != UDP_SOCKET_STATE_BOUND_NOSTREAMS) {
    abort();
  }

  udp_borrow_udp_socket_t socket_borrow = udp_borrow_udp_socket(socket->socket);

  udp_socket_streams_t *dst;
  if (remote_address != NULL) {
    dst = &socket->state.connected.streams;
  } else {
    dst = &socket->state.bound_streaming.streams;
  }

  memset(dst, 0, sizeof(*dst));

  network_error_code_t error;
  udp_tuple2_own_incoming_datagram_stream_own_outgoing_datagram_stream_t io;
  if (!udp_method_udp_socket_stream(socket_borrow, remote_address, &io, &error)) {
    errno = __wasi_sockets_utils__map_error(error);
    return -1;
  }

  dst->incoming = io.f0;
  dst->outgoing = io.f1;

  if (remote_address != NULL) {
    socket->state.tag = UDP_SOCKET_STATE_CONNECTED;
  } else {
    socket->state.tag = UDP_SOCKET_STATE_BOUND_STREAMING;
  }

  return true;
}

static void udp_close_streams(udp_socket_streams_t *streams) {
  if (streams->incoming_pollable.__handle != 0)
    poll_pollable_drop_own(streams->incoming_pollable);
  udp_incoming_datagram_stream_drop_own(streams->incoming);
  if (streams->outgoing_pollable.__handle != 0)
    poll_pollable_drop_own(streams->outgoing_pollable);
  udp_outgoing_datagram_stream_drop_own(streams->outgoing);
}

static void udp_free(void *data) {
  udp_socket_t *udp = (udp_socket_t *)data;

  switch (udp->state.tag) {
    case UDP_SOCKET_STATE_BOUND_STREAMING:
      udp_close_streams(&udp->state.bound_streaming.streams);
      break;
    case UDP_SOCKET_STATE_CONNECTED:
      udp_close_streams(&udp->state.connected.streams);
      break;
    default:
      break;
  }

  if (udp->socket_pollable.__handle != 0)
    poll_pollable_drop_own(udp->socket_pollable);
  udp_udp_socket_drop_own(udp->socket);

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

static poll_borrow_pollable_t udp_pollable(udp_socket_t *socket) {
  if (socket->socket_pollable.__handle == 0) {
    udp_borrow_udp_socket_t socket_borrow = udp_borrow_udp_socket(socket->socket);
    socket->socket_pollable = udp_method_udp_socket_subscribe(socket_borrow);
  }
  return poll_borrow_pollable(socket->socket_pollable);
}

static poll_borrow_pollable_t udp_incoming_pollable(udp_socket_streams_t *streams) {
  if (streams->incoming_pollable.__handle == 0) {
    streams->incoming_pollable = udp_method_incoming_datagram_stream_subscribe(
      udp_borrow_incoming_datagram_stream(streams->incoming));
  }
  return poll_borrow_pollable(streams->incoming_pollable);
}

static poll_borrow_pollable_t udp_outgoing_pollable(udp_socket_streams_t *streams) {
  if (streams->outgoing_pollable.__handle == 0) {
    streams->outgoing_pollable = udp_method_outgoing_datagram_stream_subscribe(
      udp_borrow_outgoing_datagram_stream(streams->outgoing));
  }
  return poll_borrow_pollable(streams->outgoing_pollable);
}

static int udp_handle_error(udp_socket_t *socket,
                            network_error_code_t error) {
  if (error == NETWORK_ERROR_CODE_WOULD_BLOCK && socket->blocking) {
    poll_method_pollable_block(udp_pollable(socket));
  } else {
    errno = __wasi_sockets_utils__map_error(error);
    return -1;
  }

  return 0;
}

static int udp_do_bind(udp_socket_t *socket, network_ip_socket_address_t *address) {
  if (socket->state.tag != UDP_SOCKET_STATE_UNBOUND) {
    errno = EINVAL;
    return -1;
  }

  network_error_code_t error;
  network_borrow_network_t network_borrow = __wasi_sockets_utils__borrow_network();
  udp_borrow_udp_socket_t socket_borrow = udp_borrow_udp_socket(socket->socket);

  if (!udp_method_udp_socket_start_bind(socket_borrow, network_borrow, address, &error)) {
    errno = __wasi_sockets_utils__map_error(error);
    return -1;
  }

  // Bind has successfully started. Attempt to finish it:
  while (!udp_method_udp_socket_finish_bind(socket_borrow, &error)) {
    if (udp_handle_error(socket, error) < 0)
      return -1;
  }

  // Bind successful.
  socket->state.tag = UDP_SOCKET_STATE_BOUND_NOSTREAMS;
  return 0;
}

static int udp_bind(void *data, const struct sockaddr *addr, socklen_t addrlen) {
  udp_socket_t *socket = (udp_socket_t *)data;
  network_ip_socket_address_t local_address;
  int parse_err;
  if (!__wasi_sockets_utils__parse_address(socket->family, addr, addrlen, &local_address, &parse_err)) {
    errno = parse_err;
    return -1;
  }
  return udp_do_bind(socket, &local_address);
}


// When `connect` is called on a UDP socket with an AF_UNSPEC address, it is
// actually a "disconnect" request.
static int udp_connect(void *data, const struct sockaddr *addr, socklen_t addrlen) {
  udp_socket_t *socket = (udp_socket_t *)data;
  if (addr == NULL || addrlen < sizeof(struct sockaddr)) {
    errno = EINVAL;
    return -1;
  }

  network_ip_socket_address_t remote_address;
  bool has_remote_address = (addr->sa_family != AF_UNSPEC);
  if (has_remote_address) {
    int parse_err;
    if (!__wasi_sockets_utils__parse_address(
          socket->family, addr, addrlen, &remote_address,
          &parse_err)) {
      errno = parse_err;
      return -1;
    }
  }

  // Prepare the socket; binding it if not bound yet, and disconnecting it if connected.
  switch (socket->state.tag) {
    case UDP_SOCKET_STATE_UNBOUND: {
      // Socket is not explicitly bound by the user. We'll do it for them:
      network_ip_socket_address_t any = __wasi_sockets_utils__any_addr(socket->family);
      if (udp_do_bind(socket, &any) < 0)
        return  -1;
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

  return udp_create_streams(socket, has_remote_address ? &remote_address : NULL);
}

static int udp_getsockname(void *data, struct sockaddr *addr, socklen_t *addrlen) {
  udp_socket_t *socket = (udp_socket_t *)data;
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

  network_error_code_t error;
  network_ip_socket_address_t result;
  udp_borrow_udp_socket_t socket_borrow = udp_borrow_udp_socket(socket->socket);
  if (!udp_method_udp_socket_local_address(socket_borrow, &result, &error)) {
    errno = __wasi_sockets_utils__map_error(error);
    return -1;
  }

  __wasi_sockets_utils__output_addr_write(result, &output_addr);
  return 0;
}

static int udp_getpeername(void *data, struct sockaddr *addr, socklen_t *addrlen) {
  udp_socket_t *socket = (udp_socket_t*) data;
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

  network_error_code_t error;
  network_ip_socket_address_t result;
  udp_borrow_udp_socket_t socket_borrow = udp_borrow_udp_socket(socket->socket);
  if (!udp_method_udp_socket_remote_address(socket_borrow, &result, &error)) {
    errno = __wasi_sockets_utils__map_error(error);
    return -1;
  }

  __wasi_sockets_utils__output_addr_write(result, &output_addr);
  return 0;
}

static ssize_t udp_recvfrom(void *data, void *buffer,
                            size_t length, int flags, struct sockaddr *addr,
                            socklen_t *addrlen)
{
  udp_socket_t *socket = (udp_socket_t*) data;
  // TODO wasi-sockets: flags:
  // - MSG_PEEK: could be shimmed by performing the receive into a local socket-specific buffer. And on subsequent receives first check that buffer.

  const int supported_flags = MSG_DONTWAIT | MSG_TRUNC;
  if ((flags & supported_flags) != flags) {
    errno = EOPNOTSUPP;
    return -1;
  }

  output_sockaddr_t output_addr;
  if (!__wasi_sockets_utils__output_addr_validate(
        socket->family, addr, addrlen, &output_addr)) {
    errno = EINVAL;
    return -1;
  }

  network_error_code_t error;
  udp_borrow_udp_socket_t socket_borrow = udp_borrow_udp_socket(socket->socket);

  // Make sure the streams are available.
  if (socket->state.tag == UDP_SOCKET_STATE_BOUND_NOSTREAMS)
    if (udp_create_streams(socket, NULL) < 0)
      return -1;

  udp_socket_streams_t *streams;
  switch (socket->state.tag) {
    case UDP_SOCKET_STATE_UNBOUND:
    case UDP_SOCKET_STATE_BOUND_NOSTREAMS:
      // Unlike `send`, `recv` should _not_ perform an implicit bind.
      errno = EINVAL;
      return -1;

    case UDP_SOCKET_STATE_BOUND_STREAMING:
      streams = &socket->state.bound_streaming.streams;
      break;

    case UDP_SOCKET_STATE_CONNECTED:
      streams = &socket->state.connected.streams;
      break;

    default: /* unreachable */
      abort();
  }

  bool return_real_size = (flags & MSG_TRUNC) != 0;
  bool should_block = socket->blocking;
  if ((flags & MSG_DONTWAIT) != 0) {
    should_block = false;
  }

  udp_borrow_incoming_datagram_stream_t incoming_borrow =
    udp_borrow_incoming_datagram_stream(streams->incoming);
  while (true) {
    udp_list_incoming_datagram_t datagrams;
    if (!udp_method_incoming_datagram_stream_receive(
          incoming_borrow, 1, &datagrams, &error)) {
      errno = __wasi_sockets_utils__map_error(error);
      return -1;
    }

    if (datagrams.len) {
      udp_incoming_datagram_t datagram = datagrams.ptr[0];
      size_t datagram_size = datagram.data.len;
      size_t bytes_to_copy =
        datagram_size < length ? datagram_size : length;

      if (output_addr.tag != OUTPUT_SOCKADDR_NULL) {
        __wasi_sockets_utils__output_addr_write(
            datagram.remote_address, &output_addr);
      }

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
}


static ssize_t udp_sendto(void *data, const void *buffer,
                          size_t length, int flags, const struct sockaddr *addr,
                          socklen_t addrlen)
{
  udp_socket_t *socket = (udp_socket_t*) data;
  const int supported_flags = MSG_DONTWAIT;
  if ((flags & supported_flags) != flags) {
    errno = EOPNOTSUPP;
    return -1;
  }

  network_ip_socket_address_t remote_address;
  bool has_remote_address = (addr != NULL);

  if (has_remote_address) {
    if (socket->state.tag == UDP_SOCKET_STATE_CONNECTED) {
      errno = EISCONN;
      return -1;
    }

    int parse_err;
    if (!__wasi_sockets_utils__parse_address(
          socket->family, addr, addrlen, &remote_address,
          &parse_err)) {
      errno = parse_err;
      return -1;
    }
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

  network_error_code_t error;
  udp_borrow_udp_socket_t socket_borrow = udp_borrow_udp_socket(socket->socket);


  // If the socket is not explicitly bound by the user we'll do it for them
  if (socket->state.tag == UDP_SOCKET_STATE_UNBOUND) {
    network_ip_socket_address_t any = __wasi_sockets_utils__any_addr(socket->family);
    if (udp_do_bind(socket, &any) < 0)
      return -1;
  }

  // Make sure the streams are available.
  if (socket->state.tag == UDP_SOCKET_STATE_BOUND_NOSTREAMS)
    if (udp_create_streams(socket, NULL) < 0)
      return -1;

  udp_socket_streams_t *streams;
  switch (socket->state.tag) {
    case UDP_SOCKET_STATE_UNBOUND:
    case UDP_SOCKET_STATE_BOUND_NOSTREAMS:
      errno = EINVAL;
      return -1;

    case UDP_SOCKET_STATE_BOUND_STREAMING:
      streams = &socket->state.bound_streaming.streams;
      break;

    case UDP_SOCKET_STATE_CONNECTED:
      streams = &socket->state.connected.streams;
      break;

    default: /* unreachable */
      abort();
  }

  bool should_block = socket->blocking;
  if ((flags & MSG_DONTWAIT) != 0) {
    should_block = false;
  }

  udp_outgoing_datagram_t datagrams[1] = {{
    .remote_address = {
      .is_some = has_remote_address,
      .val = remote_address,
    },
      .data = {
        .len = length,
        .ptr = (uint8_t*)buffer,
      },
  }};
  udp_list_outgoing_datagram_t list = {
    .len = 1,
    .ptr = datagrams,
  };

  udp_borrow_outgoing_datagram_stream_t outgoing_borrow =
    udp_borrow_outgoing_datagram_stream(streams->outgoing);
  while (true) {
    uint64_t allowed;
    if (!udp_method_outgoing_datagram_stream_check_send(
          outgoing_borrow, &allowed, &error)) {
      errno = __wasi_sockets_utils__map_error(error);
      return -1;
    }

    if (allowed) {
      uint64_t datagrams_sent;
      if (!udp_method_outgoing_datagram_stream_send(
            outgoing_borrow, &list, &datagrams_sent,
            &error)) {
        errno = __wasi_sockets_utils__map_error(error);
        return -1;
      }

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
}

static int udp_getsockopt(void *data, int level, int optname,
                          void *optval, socklen_t *optlen)
{
  udp_socket_t *socket = (udp_socket_t*) data;
  int value;

  network_error_code_t error;
  udp_borrow_udp_socket_t socket_borrow = udp_borrow_udp_socket(socket->socket);

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
          value = __wasi_sockets_utils__posix_family(socket->family);
          break;
        }
        case SO_RCVBUF: {
          uint64_t result;
          if (!udp_method_udp_socket_receive_buffer_size(
                socket_borrow, &result, &error)) {
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
          if (!udp_method_udp_socket_send_buffer_size(
                socket_borrow, &result, &error)) {
            errno = __wasi_sockets_utils__map_error(error);
            return -1;
          }

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
          if (socket->family != NETWORK_IP_ADDRESS_FAMILY_IPV4) {
            errno = EAFNOSUPPORT;
            return -1;
          }

          uint8_t result;
          if (!udp_method_udp_socket_unicast_hop_limit(
                socket_borrow, &result, &error)) {
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
          if (!udp_method_udp_socket_unicast_hop_limit(
                socket_borrow, &result, &error)) {
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
                          const void *optval, socklen_t optlen)
{
  udp_socket_t *socket = (udp_socket_t*) data;
  int intval = *(int *)optval;

  network_error_code_t error;
  udp_borrow_udp_socket_t socket_borrow =
    udp_borrow_udp_socket(socket->socket);

  switch (level) {
    case SOL_SOCKET:
      switch (optname) {
        case SO_RCVBUF: {
          if (!udp_method_udp_socket_set_receive_buffer_size(
                socket_borrow, intval, &error)) {
            errno = __wasi_sockets_utils__map_error(error);
            return -1;
          }

          return 0;
        }
        case SO_SNDBUF: {
          if (!udp_method_udp_socket_set_send_buffer_size(
                socket_borrow, intval, &error)) {
            errno = __wasi_sockets_utils__map_error(error);
            return -1;
          }

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
          if (socket->family != NETWORK_IP_ADDRESS_FAMILY_IPV4) {
            errno = EAFNOSUPPORT;
            return -1;
          }

          if (intval < 0 || intval > UINT8_MAX) {
            errno = EINVAL;
            return -1;
          }

          if (!udp_method_udp_socket_set_unicast_hop_limit(
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

          if (!udp_method_udp_socket_set_unicast_hop_limit(
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

static int udp_poll_register(void *data,
                             poll_state_t *state,
                             short events) {
  udp_socket_t *socket = (udp_socket_t *)data;
  switch (socket->state.tag) {
    case UDP_SOCKET_STATE_UNBOUND:
    case UDP_SOCKET_STATE_BOUND_NOSTREAMS:
      __wasilibc_poll_ready(state, events);
      break;

    case UDP_SOCKET_STATE_BOUND_STREAMING:
    case UDP_SOCKET_STATE_CONNECTED: {
      udp_socket_streams_t *streams;
      if (socket->state.tag == UDP_SOCKET_STATE_BOUND_STREAMING) {
        streams = &socket->state.bound_streaming.streams;
      } else {
        streams = &socket->state.connected.streams;
      }
      if ((events & POLLRDNORM) != 0) {
        if (__wasilibc_poll_add(state, POLLRDNORM, udp_incoming_pollable(streams)) < 0)
          return -1;
      }
      if ((events & POLLWRNORM) != 0) {
        if (__wasilibc_poll_add(state, POLLWRNORM, udp_outgoing_pollable(streams)) < 0)
          return -1;
      }
      break;
    }
    default:
      errno = ENOTSUP;
      return -1;
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
};

#endif // __wasilibc_use_wasip2
