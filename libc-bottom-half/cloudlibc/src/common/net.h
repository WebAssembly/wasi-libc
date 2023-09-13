// Copyright (c) 2015-2016 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#ifndef COMMON_NET_H
#define COMMON_NET_H

#include <sys/socket.h>
#include <__struct_sockaddr_in.h>
#include <__struct_sockaddr_in6.h>
#include <__struct_sockaddr_un.h>
#include <errno.h>

#include <wasi/api.h>
#include <string.h>
#include <stdint.h>
#include <netinet/in.h>
#include <unistd.h>

#ifndef MIN
#define MIN(a,b) ((a)<(b) ? (a) : (b))
#endif

static inline int is_wasi_port_ok() {
  // check if wasi port in sockaddr is not byte swapped already.
  // To find out: create a socket, bind to a port, read back and check the port
  // in case of error, return 1 as default
  int sock = socket(AF_INET, SOCK_STREAM, 0);
  if(sock<0)
    return 1;
  struct sockaddr_in addr = {0};
  addr.sin_family = AF_INET;
  addr.sin_port = 9000;
  if(bind(sock, (struct sockaddr *)&addr, sizeof(addr))<0) {
    close(sock);
    return 1;
  }
  // readback the address
  __wasi_addr_port_t local_addr;
  if(__wasi_sock_addr_local(sock, &local_addr)!=0) {
    close(sock);
    return 1;
  }
  close(sock);
  return (local_addr.u.inet4.port==10275); //10275 is 9000 with swapped bytes
}

/// Converts a WASI address into a socket address
static inline int wasi_to_sockaddr(const struct __wasi_addr_port_t *restrict peer_addr, struct sockaddr *restrict addr, socklen_t *restrict addrlen) {
  // This test needs to be here because some older versions of wasmer report the port as big-endian
  static int tested = 0;
  static int need_revert = 1;
  if(!tested && addr) {
    tested = 1;
    need_revert = is_wasi_port_ok();
  }
  if (addr != NULL) {
    memset(addr, 0, *addrlen);
    if (peer_addr->tag == __WASI_ADDRESS_FAMILY_INET4) {
      struct sockaddr_in addr4;
      addr4.sin_family = AF_INET;
      addr4.sin_port = need_revert?htons(peer_addr->u.inet4.port):peer_addr->u.inet4.port;
      addr4.sin_addr.s_addr = *((in_addr_t*)&peer_addr->u.inet4.addr);
      memcpy(addr, &addr4, MIN(sizeof(struct sockaddr_in), *addrlen));
      *addrlen = sizeof(struct sockaddr_in);
    } else if (peer_addr->tag == __WASI_ADDRESS_FAMILY_INET6) {
      struct sockaddr_in6 addr6;
      addr6.sin6_family = AF_INET6;
      addr6.sin6_flowinfo = peer_addr->u.inet6.addr.flow_info1 << 16 | peer_addr->u.inet6.addr.flow_info0;
      addr6.sin6_scope_id = peer_addr->u.inet6.addr.scope_id1 << 16 | peer_addr->u.inet6.addr.scope_id0;;
      addr6.sin6_port = need_revert?htons(peer_addr->u.inet6.port):peer_addr->u.inet6.port;
      memcpy(&addr6.sin6_addr.s6_addr, &peer_addr->u.inet6.addr, sizeof(struct in6_addr));
      memcpy(addr, &addr6, MIN(sizeof(struct sockaddr_in6), *addrlen));
      *addrlen = sizeof(struct sockaddr_in6);
    } else if (peer_addr->tag == __WASI_ADDRESS_FAMILY_UNIX) {
      struct sockaddr_un addrun;
      addrun.sun_family = AF_UNIX;
      memcpy(&addrun.sun_path, &peer_addr->u.unix.b0, sizeof(addrun.sun_path));
      addrun.sun_path[sizeof(addrun.sun_path) - 1] = '\0'; // make sure the address is null-terminated
      *addrlen = offsetof(struct sockaddr_un, sun_path) + strlen(addrun.sun_path);
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
    peer_addr->u.inet4.port = ntohs(addr4->sin_port);
    *((in_addr_t*)&peer_addr->u.inet4.addr) = addr4->sin_addr.s_addr;
    return 0;
  } else if (addr->sa_family == AF_INET6 && addrlen >= sizeof(struct sockaddr_in6)) {
    struct sockaddr_in6 *addr6 = (struct sockaddr_in6 *)addr;
    peer_addr->tag = __WASI_ADDRESS_FAMILY_INET6;
    peer_addr->u.inet6.port = ntohs(addr6->sin6_port);
    peer_addr->u.inet6.addr.flow_info1 = addr6->sin6_flowinfo >> 16;
    peer_addr->u.inet6.addr.flow_info0 = addr6->sin6_flowinfo & 0xffff;
    peer_addr->u.inet6.addr.scope_id1 = addr6->sin6_scope_id >> 16;
    peer_addr->u.inet6.addr.scope_id0 = addr6->sin6_scope_id & 0xffff;
    memcpy(&peer_addr->u.inet6.addr, &addr6->sin6_addr.s6_addr, sizeof(struct in6_addr));
    return 0;
  } else if (addr->sa_family == AF_UNIX && addrlen >= offsetof(struct sockaddr_un, sun_path) + 1) {
    struct sockaddr_un *addrun = (struct sockaddr_un *)addr;
    peer_addr->tag = __WASI_ADDRESS_FAMILY_UNIX;
    socklen_t pathlen = addrlen - offsetof(struct sockaddr_un, sun_path);
    if (pathlen > 107) { // Addresses are limited to 107 bytes + 1 null byte only
      return -1;
    }
    memcpy(&peer_addr->u.unix.b0, &addrun->sun_path, (size_t)pathlen);
    *(uint8_t *)(&peer_addr->u.unix.b0 + pathlen) = '\0';
    return 0;
  } else {
    return -1;
  }
}

#endif
