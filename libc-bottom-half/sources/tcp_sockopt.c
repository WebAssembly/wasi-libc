#include <wasi/api.h>

#ifndef __wasip1__

#include <common/time.h>
#include <errno.h>
#include <limits.h>
#include <netinet/tcp.h>
#include <stdlib.h>
#include <string.h>
#include <wasi/tcp.h>

// Normalize names on WASIp2 to the WASIp3-based names
#ifdef __wasip2__
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

#define SOCKETS_IP_ADDRESS_FAMILY_IPV4 NETWORK_IP_ADDRESS_FAMILY_IPV4
#define SOCKETS_IP_ADDRESS_FAMILY_IPV6 NETWORK_IP_ADDRESS_FAMILY_IPV6
typedef tcp_duration_t sockets_duration_t;
#endif // __wasip2__

static const uint64_t NS_PER_S = 1000000000;

int __wasilibc_tcp_getsockopt(void *data, int level, int optname,
                              void *restrict optval,
                              socklen_t *restrict optlen) {
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

      if (result > INT_MAX) {
        abort();
      }

      value = result;
      break;
    }
    case SO_SNDBUF: {
      uint64_t result;
      if (!sockets_method_tcp_socket_get_send_buffer_size(socket_borrow,
                                                          &result, &error))
        return __wasilibc_socket_error_to_errno(error);

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

      if (result_s > INT_MAX) {
        abort();
      }

      value = result_s;
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

      if (result_s > INT_MAX) {
        abort();
      }

      value = result_s;
      break;
    }
    case TCP_KEEPCNT: {
      uint32_t result;
      if (!sockets_method_tcp_socket_get_keep_alive_count(socket_borrow,
                                                          &result, &error))
        return __wasilibc_socket_error_to_errno(error);

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

int __wasilibc_tcp_setsockopt(void *data, int level, int optname,
                              const void *optval, socklen_t optlen) {
  tcp_socket_t *socket = (tcp_socket_t *)data;
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
      struct timeval *tv = (struct timeval *)optval;
      if (!timeval_to_duration(tv, &socket->recv_timeout)) {
        errno = EINVAL;
        return -1;
      }
      return 0;
    }
    case SO_SNDTIMEO: {
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
}

#endif // not(__wasip1__)
