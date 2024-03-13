#include <errno.h>
#include <netinet/in.h>

#include <wasi/api.h>
#include <wasi/descriptor_table.h>
#include <wasi/sockets_utils.h>

int tcp_bind(tcp_socket_t *socket, const struct sockaddr *addr,
	     socklen_t addrlen)
{
	network_ip_socket_address_t local_address;
	int parse_err;
	if (!__wasi_sockets_utils__parse_address(socket->family, addr, addrlen,
						 &local_address, &parse_err)) {
		errno = parse_err;
		return -1;
	}

	return __wasi_sockets_utils__tcp_bind(socket, &local_address);
}

int udp_bind(udp_socket_t *socket, const struct sockaddr *addr,
	     socklen_t addrlen)
{
	network_ip_socket_address_t local_address;
	int parse_err;
	if (!__wasi_sockets_utils__parse_address(socket->family, addr, addrlen,
						 &local_address, &parse_err)) {
		errno = parse_err;
		return -1;
	}

	return __wasi_sockets_utils__udp_bind(socket, &local_address);
}

int bind(int socket, const struct sockaddr *addr, socklen_t addrlen)
{
	descriptor_table_entry_t *entry;
	if (!descriptor_table_get_ref(socket, &entry)) {
		errno = EBADF;
		return -1;
	}

	switch (entry->tag) {
	case DESCRIPTOR_TABLE_ENTRY_TCP_SOCKET:
		return tcp_bind(&entry->tcp_socket, addr, addrlen);
	case DESCRIPTOR_TABLE_ENTRY_UDP_SOCKET:
		return udp_bind(&entry->udp_socket, addr, addrlen);
	default:
		errno = EOPNOTSUPP;
		return -1;
	}
}
