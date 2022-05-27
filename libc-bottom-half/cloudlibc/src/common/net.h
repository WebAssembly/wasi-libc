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
    if (peer_addr->tag == AF_INET) {
      struct sockaddr_in addr4;
      addr4.sin_family = AF_INET;
      addr4.sin_port = peer_addr->u.inet4.port;
      addr4.sin_addr.s_addr = *((in_addr_t*)&peer_addr->u.inet4.addr);
      memcpy(addr, &addr4, MIN(sizeof(struct sockaddr_in), *addrlen));
      *addrlen = sizeof(struct sockaddr_in);
    } else if (peer_addr->tag == AF_INET6) {
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

#endif
