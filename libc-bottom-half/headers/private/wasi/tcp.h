#ifndef WASI_TCP_H
#define WASI_TCP_H

#ifdef __wasilibc_use_wasip2

#include <wasi/wasip2.h>

/// Adds the provided TCP socket to the descriptor table, returning the
/// corresponding file descriptor.
int __wasilibc_add_tcp_socket(tcp_own_tcp_socket_t socket,
                              network_ip_address_family_t family,
                              bool blocking);

#endif // __wasilibc_use_wasip2

#endif // WASI_TCP_H
