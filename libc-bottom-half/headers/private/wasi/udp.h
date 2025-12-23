#ifndef WASI_UDP_H
#define WASI_UDP_H

#include <wasi/version.h>

#ifndef __wasip1__

#include <wasi/sockets_utils.h>

/// Adds the provided UDP socket to the descriptor table, returning the
/// corresponding file descriptor.
int __wasilibc_add_udp_socket(sockets_own_udp_socket_t socket,
                              sockets_ip_address_family_t family,
                              bool blocking);

#endif // not(__wasip1__)

#endif // WASI_UDP_H
