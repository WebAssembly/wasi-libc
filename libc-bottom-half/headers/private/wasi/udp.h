#ifndef WASI_UDP_H
#define WASI_UDP_H

#include <wasi/version.h>

#ifdef __wasip2__

#include <wasi/wasip2.h>

/// Adds the provided UDP socket to the descriptor table, returning the
/// corresponding file descriptor.
int __wasilibc_add_udp_socket(udp_own_udp_socket_t socket,
                              network_ip_address_family_t family,
                              bool blocking);

#endif // __wasip2__

#endif // WASI_UDP_H
