#include <errno.h>
#include <netinet/in.h>

#include <wasi/descriptor_table.h>
#include <wasi/sockets_utils.h>

int tcp_getsockname(tcp_socket_t *socket, struct sockaddr *addr,
		    socklen_t *addrlen)
{
	output_sockaddr_t output_addr;
	if (!__wasi_sockets_utils__output_addr_validate(
		    socket->family, addr, addrlen, &output_addr)) {
		errno = EINVAL;
		return -1;
	}

	if (output_addr.tag == OUTPUT_SOCKADDR_NULL) {
		errno = EINVAL;
		return -1;
	}

	switch (socket->state.tag) {
	case TCP_SOCKET_STATE_UNBOUND:
		errno = EINVAL;
		return -1;

	case TCP_SOCKET_STATE_BOUND:
	case TCP_SOCKET_STATE_CONNECTING:
	case TCP_SOCKET_STATE_CONNECT_FAILED:
	case TCP_SOCKET_STATE_LISTENING:
	case TCP_SOCKET_STATE_CONNECTED:
		// OK. Continue..
		break;

	default: /* unreachable */
		abort();
	}

	network_error_code_t error;
	network_ip_socket_address_t result;
	tcp_borrow_tcp_socket_t socket_borrow =
		tcp_borrow_tcp_socket(socket->socket);
	if (!tcp_method_tcp_socket_local_address(socket_borrow, &result,
						 &error)) {
		errno = __wasi_sockets_utils__map_error(error);
		return -1;
	}

	__wasi_sockets_utils__output_addr_write(result, &output_addr);

	return 0;
}

int tcp_getpeername(tcp_socket_t *socket, struct sockaddr *addr,
		    socklen_t *addrlen)
{
	output_sockaddr_t output_addr;
	if (!__wasi_sockets_utils__output_addr_validate(
		    socket->family, addr, addrlen, &output_addr)) {
		errno = EINVAL;
		return -1;
	}

	if (output_addr.tag == OUTPUT_SOCKADDR_NULL) {
		errno = EINVAL;
		return -1;
	}

	switch (socket->state.tag) {
	case TCP_SOCKET_STATE_UNBOUND:
	case TCP_SOCKET_STATE_BOUND:
	case TCP_SOCKET_STATE_CONNECTING:
	case TCP_SOCKET_STATE_CONNECT_FAILED:
	case TCP_SOCKET_STATE_LISTENING:
		errno = ENOTCONN;
		return -1;

	case TCP_SOCKET_STATE_CONNECTED:
		// OK. Continue..
		break;

	default: /* unreachable */
		abort();
	}

	network_error_code_t error;
	network_ip_socket_address_t result;
	tcp_borrow_tcp_socket_t socket_borrow =
		tcp_borrow_tcp_socket(socket->socket);
	if (!tcp_method_tcp_socket_remote_address(socket_borrow, &result,
						  &error)) {
		errno = __wasi_sockets_utils__map_error(error);
		return -1;
	}

	__wasi_sockets_utils__output_addr_write(result, &output_addr);

	return 0;
}

int udp_getsockname(udp_socket_t *socket, struct sockaddr *addr,
		    socklen_t *addrlen)
{
	output_sockaddr_t output_addr;
	if (!__wasi_sockets_utils__output_addr_validate(
		    socket->family, addr, addrlen, &output_addr)) {
		errno = EINVAL;
		return -1;
	}

	if (output_addr.tag == OUTPUT_SOCKADDR_NULL) {
		errno = EINVAL;
		return -1;
	}

	switch (socket->state.tag) {
	case UDP_SOCKET_STATE_UNBOUND:
		errno = EINVAL;
		return -1;

	case UDP_SOCKET_STATE_BOUND_NOSTREAMS:
	case UDP_SOCKET_STATE_BOUND_STREAMING:
	case UDP_SOCKET_STATE_CONNECTED:
		// OK. Continue..
		break;

	default: /* unreachable */
		abort();
	}

	network_error_code_t error;
	network_ip_socket_address_t result;
	udp_borrow_udp_socket_t socket_borrow =
		udp_borrow_udp_socket(socket->socket);
	if (!udp_method_udp_socket_local_address(socket_borrow, &result,
						 &error)) {
		errno = __wasi_sockets_utils__map_error(error);
		return -1;
	}

	__wasi_sockets_utils__output_addr_write(result, &output_addr);

	return 0;
}

int udp_getpeername(udp_socket_t *socket, struct sockaddr *addr,
		    socklen_t *addrlen)
{
	output_sockaddr_t output_addr;
	if (!__wasi_sockets_utils__output_addr_validate(
		    socket->family, addr, addrlen, &output_addr)) {
		errno = EINVAL;
		return -1;
	}

	if (output_addr.tag == OUTPUT_SOCKADDR_NULL) {
		errno = EINVAL;
		return -1;
	}

	switch (socket->state.tag) {
	case UDP_SOCKET_STATE_UNBOUND:
	case UDP_SOCKET_STATE_BOUND_NOSTREAMS:
	case UDP_SOCKET_STATE_BOUND_STREAMING:
		errno = ENOTCONN;
		return -1;

	case UDP_SOCKET_STATE_CONNECTED:
		// OK. Continue..
		break;

	default: /* unreachable */
		abort();
	}

	network_error_code_t error;
	network_ip_socket_address_t result;
	udp_borrow_udp_socket_t socket_borrow =
		udp_borrow_udp_socket(socket->socket);
	if (!udp_method_udp_socket_remote_address(socket_borrow, &result,
						  &error)) {
		errno = __wasi_sockets_utils__map_error(error);
		return -1;
	}

	__wasi_sockets_utils__output_addr_write(result, &output_addr);

	return 0;
}

int getsockname(int socket, struct sockaddr *__restrict addr,
		socklen_t *__restrict addrlen)
{
	descriptor_table_entry_t *entry;
	if (!descriptor_table_get_ref(socket, &entry)) {
		errno = EBADF;
		return -1;
	}

	switch (entry->tag) {
	case DESCRIPTOR_TABLE_ENTRY_TCP_SOCKET:
		return tcp_getsockname(&entry->tcp_socket, addr, addrlen);
	case DESCRIPTOR_TABLE_ENTRY_UDP_SOCKET:
		return udp_getsockname(&entry->udp_socket, addr, addrlen);
	default:
		errno = EOPNOTSUPP;
		return -1;
	}
}

int getpeername(int socket, struct sockaddr *__restrict addr,
		socklen_t *__restrict addrlen)
{
	descriptor_table_entry_t *entry;
	if (!descriptor_table_get_ref(socket, &entry)) {
		errno = EBADF;
		return -1;
	}

	switch (entry->tag) {
	case DESCRIPTOR_TABLE_ENTRY_TCP_SOCKET:
		return tcp_getpeername(&entry->tcp_socket, addr, addrlen);
	case DESCRIPTOR_TABLE_ENTRY_UDP_SOCKET:
		return udp_getpeername(&entry->udp_socket, addr, addrlen);
	default:
		errno = EOPNOTSUPP;
		return -1;
	}
}
