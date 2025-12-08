#include <errno.h>
#include <netinet/in.h>
#include <wasi/descriptor_table.h>
#include <wasi/sockets_utils.h>
#include <wasi/tcp.h>
#include <wasi/udp.h>

static int tcp_socket(network_ip_address_family_t family, bool blocking)
{
	tcp_create_socket_error_code_t error;
	tcp_own_tcp_socket_t socket;
	if (!tcp_create_socket_create_tcp_socket(family, &socket, &error)) {
		errno = __wasi_sockets_utils__map_error(error);
		return -1;
	}

        return __wasilibc_add_tcp_socket(socket, family, blocking);
}

static int udp_socket(network_ip_address_family_t family, bool blocking)
{
	udp_create_socket_error_code_t error;
	udp_own_udp_socket_t socket;
	if (!udp_create_socket_create_udp_socket(family, &socket, &error)) {
		errno = __wasi_sockets_utils__map_error(error);
		return -1;
	}

        return __wasilibc_add_udp_socket(socket, family, blocking);
}

int socket(int domain, int type, int protocol)
{
	network_ip_address_family_t family;
	switch (domain) {
	case PF_INET:
		family = NETWORK_IP_ADDRESS_FAMILY_IPV4;
		break;

	case PF_INET6:
		family = NETWORK_IP_ADDRESS_FAMILY_IPV6;
		break;

	default:
		errno = EAFNOSUPPORT;
		return -1;
	}

	int real_type = type & ~(SOCK_NONBLOCK | SOCK_CLOEXEC);
	bool blocking = (type & SOCK_NONBLOCK) == 0;
	// Ignore SOCK_CLOEXEC flag. That concept does not exist in WASI.

	if (real_type == SOCK_STREAM &&
	    (protocol == 0 || protocol == IPPROTO_TCP)) {
		return tcp_socket(family, blocking);

	} else if (real_type == SOCK_DGRAM &&
		   (protocol == 0 || protocol == IPPROTO_UDP)) {
		return udp_socket(family, blocking);

	} else {
		errno = EPROTONOSUPPORT;
		return -1;
	}
}
