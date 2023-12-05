#ifndef __wasi_sockets_utils_h
#define __wasi_sockets_utils_h

#include <netinet/in.h>

#include <descriptor_table.h>

network_borrow_network_t __wasi_sockets_utils__borrow_network();
int __wasi_sockets_utils__map_error(network_error_code_t wasi_error);
bool __wasi_sockets_utils__parse_address(const struct sockaddr* address, socklen_t len, network_ip_socket_address_t* output, int* error);
bool __wasi_sockets_utils__format_address(const network_ip_socket_address_t* address, struct sockaddr* output_addr, socklen_t* output_addrlen, int* error);
int __wasi_sockets_utils__posix_family(network_ip_address_family_t wasi_family);
network_ip_socket_address_t __wasi_sockets_utils__any_addr(network_ip_address_family_t family);
int __wasi_sockets_utils__tcp_bind(tcp_socket_t* socket, network_ip_socket_address_t* address);

#endif