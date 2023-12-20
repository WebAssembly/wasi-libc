// Copyright (c) 2015-2016 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <sys/socket.h>
#include <netinet/in.h>
#include <assert.h>
#include <errno.h>
#include <limits.h>
#include <string.h>
#include <wasi/api.h>

#ifdef __wasilibc_use_preview2
#include <descriptor_table.h>
#include "__utils.h"

const uint64_t NS_PER_S = 1000000000;

int tcp_getsockopt(tcp_socket_t* socket, int level, int optname,
    void* restrict optval, socklen_t* restrict optlen)
{
    int value = 0;

    network_error_code_t error;
    tcp_borrow_tcp_socket_t socket_borrow = tcp_borrow_tcp_socket(socket->socket);

    switch (level)
    {
    case SOL_SOCKET:
        switch (optname) {
        case SO_TYPE: {
            value = SOCK_STREAM;
            break;
        }
        case SO_PROTOCOL: {
            value = IPPROTO_TCP;
            break;
        }
        case SO_DOMAIN: {
            value = __wasi_sockets_utils__posix_family(socket->family);
            break;
        }
        case SO_ERROR: {
            if (socket->state.tag == TCP_SOCKET_STATE_CONNECT_FAILED) {
                value = __wasi_sockets_utils__map_error(socket->state.connect_failed.error_code);
                socket->state.connect_failed.error_code = 0;
            } else {
                value = 0;
            }
            break;
        }
        case SO_ACCEPTCONN: {
            bool is_listening = socket->state.tag == TCP_SOCKET_STATE_LISTENING;
            assert(is_listening == tcp_method_tcp_socket_is_listening(socket_borrow)); // Sanity check.
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
        case SO_REUSEADDR: {
            value = socket->fake_reuseaddr;
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
            if (socket->family != NETWORK_IP_ADDRESS_FAMILY_IPV4)
            {
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
            if (socket->family != NETWORK_IP_ADDRESS_FAMILY_IPV6)
            {
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
        case IPV6_V6ONLY: {
            bool result;
            if (!tcp_method_tcp_socket_ipv6_only(socket_borrow, &result, &error)) {
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
            if (!tcp_method_tcp_socket_keep_alive_idle_time(socket_borrow, &result_ns, &error)) {
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
    memcpy(optval, &value,
        *optlen < sizeof(int) ? *optlen : sizeof(int));
    *optlen = sizeof(int);
    return 0;
}

int tcp_setsockopt(tcp_socket_t* socket, int level, int optname, const void* optval, socklen_t optlen) {
    
    int intval = *(int*)optval;
    
    network_error_code_t error;
    tcp_borrow_tcp_socket_t socket_borrow = tcp_borrow_tcp_socket(socket->socket);

    switch (level)
    {
    case SOL_SOCKET:
        switch (optname) {
        case SO_KEEPALIVE: {
            if (!tcp_method_tcp_socket_set_keep_alive_enabled(socket_borrow, intval != 0, &error)) {
                errno = __wasi_sockets_utils__map_error(error);
                return -1;
            }

            return 0;
        }
        case SO_RCVBUF: {
            if (!tcp_method_tcp_socket_set_receive_buffer_size(socket_borrow, intval, &error)) {
                errno = __wasi_sockets_utils__map_error(error);
                return -1;
            }

            return 0;
        }
        case SO_SNDBUF: {
            if (!tcp_method_tcp_socket_set_send_buffer_size(socket_borrow, intval, &error)) {
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
            if (socket->family != NETWORK_IP_ADDRESS_FAMILY_IPV4)
            {
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
            if (socket->family != NETWORK_IP_ADDRESS_FAMILY_IPV6)
            {
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
        case IPV6_V6ONLY: {
            if (!tcp_method_tcp_socket_set_ipv6_only(socket_borrow, intval != 0, &error)) {
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
            if (!tcp_method_tcp_socket_set_keep_alive_idle_time(socket_borrow, duration, &error)) {
                errno = __wasi_sockets_utils__map_error(error);
                return -1;
            }

            return 0;
        }
        case TCP_KEEPINTVL: {
            tcp_duration_t duration = intval * NS_PER_S;
            if (!tcp_method_tcp_socket_set_keep_alive_interval(socket_borrow, duration, &error)) {
                errno = __wasi_sockets_utils__map_error(error);
                return -1;
            }

            return 0;
        }
        case TCP_KEEPCNT: {
            if (!tcp_method_tcp_socket_set_keep_alive_count(socket_borrow, intval, &error)) {
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

int udp_getsockopt(udp_socket_t* socket, int level, int optname,
    void* restrict optval, socklen_t* restrict optlen)
{
    int value;

    network_error_code_t error;
    udp_borrow_udp_socket_t socket_borrow = udp_borrow_udp_socket(socket->socket);

    switch (level)
    {
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
            if (!udp_method_udp_socket_receive_buffer_size(socket_borrow, &result, &error)) {
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
            if (!udp_method_udp_socket_send_buffer_size(socket_borrow, &result, &error)) {
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
            if (socket->family != NETWORK_IP_ADDRESS_FAMILY_IPV4)
            {
                errno = EAFNOSUPPORT;
                return -1;
            }

            uint8_t result;
            if (!udp_method_udp_socket_unicast_hop_limit(socket_borrow, &result, &error)) {
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
            if (socket->family != NETWORK_IP_ADDRESS_FAMILY_IPV6)
            {
                errno = EAFNOSUPPORT;
                return -1;
            }

            uint8_t result;
            if (!udp_method_udp_socket_unicast_hop_limit(socket_borrow, &result, &error)) {
                errno = __wasi_sockets_utils__map_error(error);
                return -1;
            }

            value = result;
            break;
        }
        case IPV6_V6ONLY: {
            bool result;
            if (!udp_method_udp_socket_ipv6_only(socket_borrow, &result, &error)) {
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
    memcpy(optval, &value,
        *optlen < sizeof(int) ? *optlen : sizeof(int));
    *optlen = sizeof(int);
    return 0;
}

int udp_setsockopt(udp_socket_t* socket, int level, int optname, const void* optval, socklen_t optlen) {

    int intval = *(int*)optval;
    
    network_error_code_t error;
    udp_borrow_udp_socket_t socket_borrow = udp_borrow_udp_socket(socket->socket);

    switch (level)
    {
    case SOL_SOCKET:
        switch (optname) {
        case SO_RCVBUF: {
            if (!udp_method_udp_socket_set_receive_buffer_size(socket_borrow, intval, &error)) {
                errno = __wasi_sockets_utils__map_error(error);
                return -1;
            }

            return 0;
        }
        case SO_SNDBUF: {
            if (!udp_method_udp_socket_set_send_buffer_size(socket_borrow, intval, &error)) {
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
            if (socket->family != NETWORK_IP_ADDRESS_FAMILY_IPV4)
            {
                errno = EAFNOSUPPORT;
                return -1;
            }

            if (intval < 0 || intval > UINT8_MAX) {
                errno = EINVAL;
                return -1;
            }

            if (!udp_method_udp_socket_set_unicast_hop_limit(socket_borrow, intval, &error)) {
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
            if (socket->family != NETWORK_IP_ADDRESS_FAMILY_IPV6)
            {
                errno = EAFNOSUPPORT;
                return -1;
            }

            if (intval < 0 || intval > UINT8_MAX) {
                errno = EINVAL;
                return -1;
            }

            if (!udp_method_udp_socket_set_unicast_hop_limit(socket_borrow, intval, &error)) {
                errno = __wasi_sockets_utils__map_error(error);
                return -1;
            }

            return 0;
        }
        case IPV6_V6ONLY: {
            if (!udp_method_udp_socket_set_ipv6_only(socket_borrow, intval != 0, &error)) {
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

int getsockopt(int sockfd, int level, int optname,
    void* restrict optval, socklen_t* restrict optlen)
{
    descriptor_table_entry_t* entry;
    if (!descriptor_table_get_ref(sockfd, &entry)) {
        errno = EBADF;
        return -1;
    }

    if (optval == NULL || optlen == NULL || *optlen < sizeof(int)) {
        // FYI, the protocol-specific implementations implicitly depend on these checks.
        errno = EINVAL;
        return -1;
    }

    switch (entry->tag)
    {
    case DESCRIPTOR_TABLE_ENTRY_TCP_SOCKET:
        return tcp_getsockopt(&entry->tcp_socket, level, optname, optval, optlen);
    case DESCRIPTOR_TABLE_ENTRY_UDP_SOCKET:
        return udp_getsockopt(&entry->udp_socket, level, optname, optval, optlen);
    default:
        errno = ENOPROTOOPT;
        return -1;
    }
}

int setsockopt(int sockfd, int level, int optname, const void* optval, socklen_t optlen) {

    descriptor_table_entry_t* entry;
    if (!descriptor_table_get_ref(sockfd, &entry)) {
        errno = EBADF;
        return -1;
    }

    if (optval == NULL || optlen < sizeof(int)) {
        // FYI, the protocol-specific implementations implicitly depend on these checks.
        errno = EINVAL;
        return -1;
    }

    switch (entry->tag)
    {
    case DESCRIPTOR_TABLE_ENTRY_TCP_SOCKET:
        return tcp_setsockopt(&entry->tcp_socket, level, optname, optval, optlen);
    case DESCRIPTOR_TABLE_ENTRY_UDP_SOCKET:
        return udp_setsockopt(&entry->udp_socket, level, optname, optval, optlen);
    default:
        errno = ENOPROTOOPT;
        return -1;
    }
}

#else // __wasilibc_use_preview2

int getsockopt(int socket, int level, int option_name,
               void *restrict option_value, socklen_t *restrict option_len) {
  // Only support SOL_SOCKET options for now.
  if (level != SOL_SOCKET) {
    errno = ENOPROTOOPT;
    return -1;
  }

  int value;
  switch (option_name) {
    case SO_TYPE: {
      // Return the type of the socket. This information can simply be
      // obtained by looking at the file descriptor type.
      __wasi_fdstat_t fsb;
      if (__wasi_fd_fdstat_get(socket, &fsb) != 0) {
        errno = EBADF;
        return -1;
      }
      if (fsb.fs_filetype != __WASI_FILETYPE_SOCKET_DGRAM &&
          fsb.fs_filetype != __WASI_FILETYPE_SOCKET_STREAM) {
        errno = ENOTSOCK;
        return -1;
      }
      value = fsb.fs_filetype;
      break;
    }
    default: {
      errno = ENOPROTOOPT;
      return -1;
    }
  }

  // Copy out integer value.
  memcpy(option_value, &value,
         *option_len < sizeof(int) ? *option_len : sizeof(int));
  *option_len = sizeof(int);
  return 0;
}

#endif // __wasilibc_use_preview2