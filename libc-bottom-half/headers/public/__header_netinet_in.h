#ifndef __wasilibc___header_netinet_in_h
#define __wasilibc___header_netinet_in_h

#include <__struct_in_addr.h>
#include <__struct_in6_addr.h>
#include <__struct_sockaddr_in.h>
#include <__struct_sockaddr_in6.h>

#define IPPROTO_IP __WASI_IP_PROTO_IPPROTO_IP
#define IPPROTO_ICMP __WASI_IP_PROTO_IPPROTO_ICMP
#define IPPROTO_TCP __WASI_IP_PROTO_IPPROTO_TCP
#define IPPROTO_UDP __WASI_IP_PROTO_IPPROTO_UDP
#define IPPROTO_IPV6 __WASI_IP_PROTO_IPPROTO_IPV6
#define IPPROTO_RAW __WASI_IP_PROTO_IPPROTO_RAW

#define IN6ADDR_ANY_INIT { { \
    0x00, 0x00, 0x00, 0x00, \
    0x00, 0x00, 0x00, 0x00, \
    0x00, 0x00, 0x00, 0x00, \
    0x00, 0x00, 0x00, 0x00  \
} }

#define IN6ADDR_LOOPBACK_INIT { { \
    0x00, 0x00, 0x00, 0x00, \
    0x00, 0x00, 0x00, 0x00, \
    0x00, 0x00, 0x00, 0x00, \
    0x00, 0x00, 0x00, 0x01  \
} }

#endif
