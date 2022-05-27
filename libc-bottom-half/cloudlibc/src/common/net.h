// Copyright (c) 2015-2016 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#ifndef COMMON_NET_H
#define COMMON_NET_H

#include <sys/socket.h>
#include <__struct_sockaddr_in.h>
#include <__struct_sockaddr_in6.h>
#include <common/errno.h>

#include <wasi/api.h>
#include <string.h>
#include <stdint.h>

#ifndef MIN
#define MIN(a,b) ((a)<(b) ? (a) : (b))
#endif

/// Converts a WASI address into a socket address
static inline int wasi_to_sockaddr(const struct __wasi_addr_port_t *restrict peer_addr, struct sockaddr *restrict addr, socklen_t *restrict addrlen) {
  if (addr != NULL) {
    memset(addr, 0, *addrlen);
    if (peer_addr->tag == __WASI_ADDRESS_FAMILY_INET4) {
      struct sockaddr_in addr4;
      addr4.sin_family = AF_INET;
      addr4.sin_port = peer_addr->u.inet4.port;
      addr4.sin_addr.s_addr = *((in_addr_t*)&peer_addr->u.inet4.addr);
      memcpy(addr, &addr4, MIN(sizeof(struct sockaddr_in), *addrlen));
      *addrlen = sizeof(struct sockaddr_in);
    } else if (peer_addr->tag == __WASI_ADDRESS_FAMILY_INET6) {
      struct sockaddr_in6 addr6;
      addr6.sin6_family = AF_INET6;
      addr6.sin6_flowinfo = 0;
      addr6.sin6_scope_id = 0;
      addr6.sin6_port = peer_addr->u.inet6.port;
      memcpy(&addr6.sin6_addr.s6_addr, &peer_addr->u.inet6.addr, sizeof(struct in6_addr));
      memcpy(addr, &addr6, MIN(sizeof(struct sockaddr_in6), *addrlen));
      *addrlen = sizeof(struct sockaddr_in6);
    } else {
      addr->sa_family = AF_UNSPEC;
      *addrlen = sizeof(struct sockaddr);
    }
    return 0;
  }
  return -1;
}

static inline int sockaddr_to_wasi(const struct sockaddr *restrict addr, const socklen_t addrlen, struct __wasi_addr_port_t *restrict peer_addr) {
  memset(peer_addr, 0, sizeof(struct __wasi_addr_port_t));
  if (addr->sa_family == AF_INET && addrlen >= sizeof(struct sockaddr_in)) {
    struct sockaddr_in *addr4 = (struct sockaddr_in *)addr;
    peer_addr->tag = __WASI_ADDRESS_FAMILY_INET4;
    peer_addr->u.inet4.port = addr4->sin_port;
    *((in_addr_t*)&peer_addr->u.inet4.addr) = addr4->sin_addr.s_addr;
    return 0;
  } else if (addr->sa_family == AF_INET6 && addrlen >= sizeof(struct sockaddr_in6)) {
    struct sockaddr_in6 *addr6 = (struct sockaddr_in6 *)addr;
    peer_addr->tag = __WASI_ADDRESS_FAMILY_INET6;
    peer_addr->u.inet6.port = addr6->sin6_port;
    memcpy(&peer_addr->u.inet6.addr, &addr6->sin6_addr.s6_addr, sizeof(struct in6_addr));
    return 0;
  } else {
    return -1;
  }
}

#endif
