#ifndef WASI_TCP_H
#define WASI_TCP_H

#include <wasi/version.h>

#ifndef __wasip1__

#include <wasi/sockets_utils.h>
#include <sys/socket.h>

// Normalize names on WASIp2 to the WASIp3-based names
#ifdef __wasip2__
typedef network_error_code_t sockets_error_code_t;
typedef tcp_own_tcp_socket_t sockets_own_tcp_socket_t;
typedef tcp_borrow_tcp_socket_t sockets_borrow_tcp_socket_t;
typedef network_ip_address_family_t sockets_ip_address_family_t;
#define sockets_borrow_tcp_socket tcp_borrow_tcp_socket
#endif // __wasip2__

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
#ifdef __wasip2__
  streams_own_input_stream_t input;
  poll_own_pollable_t input_pollable;
  streams_own_output_stream_t output;
  poll_own_pollable_t output_pollable;
#else
  int dummy; // TODO(wasip3)
#endif
} tcp_socket_state_connected_t;

typedef struct {
  sockets_error_code_t error_code;
} tcp_socket_state_connect_failed_t;

// This is a tagged union. When adding/removing/renaming cases, be sure to keep
// the tag and union definitions in sync.
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
  sockets_own_tcp_socket_t socket;
  tcp_socket_state_t state;
#ifdef __wasip2__
  // This pollable is lazily initialized on-demand.
  poll_own_pollable_t socket_pollable;
#endif
  bool blocking;
  bool fake_nodelay;
  bool fake_reuseaddr;
  sockets_ip_address_family_t family;
  monotonic_clock_duration_t send_timeout;
  monotonic_clock_duration_t recv_timeout;
} tcp_socket_t;

int __wasilibc_tcp_getsockopt(void *data, int level, int optname,
                              void *restrict optval, socklen_t *restrict optlen);
int __wasilibc_tcp_setsockopt(void *data, int level, int optname,
                              const void *optval, socklen_t optlen);

/// Adds the provided TCP socket to the descriptor table, returning the
/// corresponding file descriptor.
int __wasilibc_add_tcp_socket(sockets_own_tcp_socket_t socket,
                              sockets_ip_address_family_t family,
                              bool blocking);

#endif // not(__wasip1__)

#endif // WASI_TCP_H
