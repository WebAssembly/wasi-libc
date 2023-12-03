#include <errno.h>
#include <netinet/in.h>

#include <descriptor_table.h>
#include "__utils.h"

int tcp_bind(tcp_socket_t* socket, wasi_sockets_0_2_0_rc_2023_10_18_network_ip_socket_address_t* address) {

    tcp_socket_state_unbound_t unbound;
    if (socket->state_tag == TCP_SOCKET_STATE_UNBOUND) {
        unbound = socket->state.unbound;
    } else {
        errno = EINVAL;
        return -1;
    }

	wasi_sockets_0_2_0_rc_2023_10_18_network_error_code_t error;
	reactor_borrow_network_t network_borrow = __wasi_sockets_utils__borrow_network();
    reactor_borrow_tcp_socket_t socket_borrow = wasi_sockets_0_2_0_rc_2023_10_18_tcp_borrow_tcp_socket(socket->socket);

	if (!wasi_sockets_0_2_0_rc_2023_10_18_tcp_method_tcp_socket_start_bind(socket_borrow, network_borrow, address, &error)) {
        errno = __wasi_sockets_utils__map_error(error);
        return -1;
    }

    // Bind has successfully started. Attempt to finish it:
    while (!wasi_sockets_0_2_0_rc_2023_10_18_tcp_method_tcp_socket_finish_bind(socket_borrow, &error)) {
        if (error == WASI_SOCKETS_0_2_0_RC_2023_10_18_NETWORK_ERROR_CODE_WOULD_BLOCK) {
			reactor_borrow_pollable_t pollable_borrow = wasi_io_0_2_0_rc_2023_10_18_poll_borrow_pollable(socket->socket_pollable);
			wasi_io_0_2_0_rc_2023_10_18_poll_poll_one(pollable_borrow);
        } else {
            errno = __wasi_sockets_utils__map_error(error);
            return -1;
        }
    }

    // Bind successful.

    socket->state_tag = TCP_SOCKET_STATE_BOUND;
    socket->state = (tcp_socket_state_t){ .bound = { /* No additional state */ } };
    return 0;
}

int udp_bind(udp_socket_t* socket, wasi_sockets_0_2_0_rc_2023_10_18_network_ip_socket_address_t* address) {
    // TODO: implement
	errno = EOPNOTSUPP;
	return -1;
}

int bind(int socket, const struct sockaddr* addr, socklen_t addrlen) {

    descriptor_table_entry_t* entry;
    if (!descriptor_table_get_ref(socket, &entry)) {
        errno = EBADF;
        return -1;
    }

	wasi_sockets_0_2_0_rc_2023_10_18_network_ip_socket_address_t ip_address;
    int parse_err;
    if (!__wasi_sockets_utils__parse_address(addr, addrlen, &ip_address, &parse_err)) {
        errno = parse_err;
        return -1;
    }

    switch (entry->tag)
    {
    case DESCRIPTOR_TABLE_ENTRY_TCP_SOCKET:
        return tcp_bind(&entry->value.tcp_socket, &ip_address);
    case DESCRIPTOR_TABLE_ENTRY_UDP_SOCKET:
        return udp_bind(&entry->value.udp_socket, &ip_address);
    default:
        errno = EOPNOTSUPP;
        return -1;
    }
}
