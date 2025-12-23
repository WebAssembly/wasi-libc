#ifndef WASI_TCP_H
#define WASI_TCP_H

#include <wasi/version.h>

#ifndef __wasip1__

#include <wasi/sockets_utils.h>

/// Adds the provided TCP socket to the descriptor table, returning the
/// corresponding file descriptor.
int __wasilibc_add_tcp_socket(sockets_own_tcp_socket_t socket,
                              sockets_ip_address_family_t family,
                              bool blocking);

#endif // not(__wasip1__)

#endif // WASI_TCP_H
