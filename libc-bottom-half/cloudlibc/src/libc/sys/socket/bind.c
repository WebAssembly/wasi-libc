#include <errno.h>
#include <netinet/in.h>

#include <descriptor_table.h>
#include "__utils.h"

int tcp_bind(tcp_socket_t* socket, network_ip_socket_address_t* address) {
    return __wasi_sockets_utils__tcp_bind(socket, address);
}

int udp_bind(udp_socket_t* socket, network_ip_socket_address_t* address) {
    // TODO wasi-sockets: implement
	errno = EOPNOTSUPP;
	return -1;
}

int bind(int socket, const struct sockaddr* addr, socklen_t addrlen) {

    descriptor_table_entry_t* entry;
    if (!descriptor_table_get_ref(socket, &entry)) {
        errno = EBADF;
        return -1;
    }

	network_ip_socket_address_t ip_address;
    int parse_err;
    if (!__wasi_sockets_utils__parse_address(addr, addrlen, &ip_address, &parse_err)) {
        errno = parse_err;
        return -1;
    }

    switch (entry->tag)
    {
    case DESCRIPTOR_TABLE_ENTRY_TCP_SOCKET:
        return tcp_bind(&entry->tcp_socket, &ip_address);
    case DESCRIPTOR_TABLE_ENTRY_UDP_SOCKET:
        return udp_bind(&entry->udp_socket, &ip_address);
    default:
        errno = EOPNOTSUPP;
        return -1;
    }
}
