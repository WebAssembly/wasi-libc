#include <sys/socket.h>

#include <errno.h>

#include <wasi/api.h>
#include <wasi/descriptor_table.h>
#include <wasi/sockets_utils.h>

static ssize_t tcp_sendto(tcp_socket_t *socket, const uint8_t *buffer,
			  size_t length, int flags, const struct sockaddr *addr,
			  socklen_t addrlen)
{
	const int supported_flags = MSG_DONTWAIT | MSG_NOSIGNAL;
	if ((flags & supported_flags) != flags) {
		errno = EOPNOTSUPP;
		return -1;
	}

	if (addr != NULL || addrlen != 0) {
		errno = EISCONN;
		return -1;
	}

	tcp_socket_state_connected_t connection;
	if (socket->state.tag == TCP_SOCKET_STATE_CONNECTED) {
		connection = socket->state.connected;
	} else {
		errno = ENOTCONN;
		return -1;
	}

	bool should_block = socket->blocking;
	if ((flags & MSG_DONTWAIT) != 0) {
		should_block = false;
	}

	if ((flags & MSG_NOSIGNAL) != 0) {
		// Ignore it. WASI has no Unix-style signals. So effectively,
		// MSG_NOSIGNAL is always the case, whether it was explicitly
		// requested or not.
	}

	streams_borrow_output_stream_t tx_borrow =
		streams_borrow_output_stream(connection.output);
	while (true) {
		streams_stream_error_t error;
		uint64_t count;
		if (!streams_method_output_stream_check_write(tx_borrow, &count,
							      &error)) {
			// TODO wasi-sockets: wasi-sockets has no way to recover stream errors yet.
			errno = EPIPE;
			return -1;
		}

		if (count) {
			count = count < length ? count : length;
			wasip2_list_u8_t list = { .ptr = (uint8_t *)buffer,
						    .len = count };
			if (!streams_method_output_stream_write(
				    tx_borrow, &list, &error)) {
				// TODO wasi-sockets: wasi-sockets has no way to recover TCP stream errors yet.
				errno = EPIPE;
				return -1;
			} else {
				return count;
			}
		} else if (should_block) {
			poll_borrow_pollable_t pollable_borrow =
				poll_borrow_pollable(
					connection.output_pollable);
			poll_method_pollable_block(pollable_borrow);
		} else {
			errno = EWOULDBLOCK;
			return -1;
		}
	}
}

static ssize_t udp_sendto(udp_socket_t *socket, const uint8_t *buffer,
			  size_t length, int flags, const struct sockaddr *addr,
			  socklen_t addrlen)
{
	const int supported_flags = MSG_DONTWAIT;
	if ((flags & supported_flags) != flags) {
		errno = EOPNOTSUPP;
		return -1;
	}

	network_ip_socket_address_t remote_address;
	bool has_remote_address = (addr != NULL);

	if (has_remote_address) {
		if (socket->state.tag == UDP_SOCKET_STATE_CONNECTED) {
			errno = EISCONN;
			return -1;
		}

		int parse_err;
		if (!__wasi_sockets_utils__parse_address(
			    socket->family, addr, addrlen, &remote_address,
			    &parse_err)) {
			errno = parse_err;
			return -1;
		}
	} else {
		if (addrlen != 0) {
			errno = EINVAL;
			return -1;
		}

		if (socket->state.tag != UDP_SOCKET_STATE_CONNECTED) {
			errno = EDESTADDRREQ;
			return -1;
		}
	}

	network_error_code_t error;
	udp_borrow_udp_socket_t socket_borrow =
		udp_borrow_udp_socket(socket->socket);

	udp_socket_streams_t streams;
	switch (socket->state.tag) {
	case UDP_SOCKET_STATE_UNBOUND: {
		// Socket is not explicitly bound by the user. We'll do it for them:

		network_ip_socket_address_t any =
			__wasi_sockets_utils__any_addr(socket->family);
		int result = __wasi_sockets_utils__udp_bind(socket, &any);
		if (result != 0) {
			return result;
		}

		if (!__wasi_sockets_utils__stream(socket, NULL, &streams,
						  &error)) {
			errno = __wasi_sockets_utils__map_error(error);
			return -1;
		}
		break;
	}
	case UDP_SOCKET_STATE_BOUND_NOSTREAMS: {
		if (!__wasi_sockets_utils__stream(socket, NULL, &streams,
						  &error)) {
			errno = __wasi_sockets_utils__map_error(error);
			return -1;
		}
		break;
	}
	case UDP_SOCKET_STATE_BOUND_STREAMING:
		streams = socket->state.bound_streaming.streams;
		break;

	case UDP_SOCKET_STATE_CONNECTED:
		streams = socket->state.connected.streams;
		break;

	default: /* unreachable */
		abort();
	}

	bool should_block = socket->blocking;
	if ((flags & MSG_DONTWAIT) != 0) {
		should_block = false;
	}

	udp_outgoing_datagram_t datagrams[1] = {{
        .remote_address = {
            .is_some = has_remote_address,
            .val = remote_address,
        },
        .data = {
            .len = length,
            .ptr = (uint8_t*)buffer,
        },
    }};
	udp_list_outgoing_datagram_t list = {
		.len = 1,
		.ptr = datagrams,
	};

	udp_borrow_outgoing_datagram_stream_t outgoing_borrow =
		udp_borrow_outgoing_datagram_stream(streams.outgoing);
	while (true) {
		uint64_t allowed;
		if (!udp_method_outgoing_datagram_stream_check_send(
			    outgoing_borrow, &allowed, &error)) {
			errno = __wasi_sockets_utils__map_error(error);
			return -1;
		}

		if (allowed) {
			uint64_t datagrams_sent;
			if (!udp_method_outgoing_datagram_stream_send(
				    outgoing_borrow, &list, &datagrams_sent,
				    &error)) {
				errno = __wasi_sockets_utils__map_error(error);
				return -1;
			}

			if (datagrams_sent != 0 && datagrams_sent != 1) {
                            abort();
                        }

			if (datagrams_sent == 1) {
				return length;
			}
		}

		if (should_block) {
			poll_borrow_pollable_t pollable_borrow =
				poll_borrow_pollable(streams.outgoing_pollable);
			poll_method_pollable_block(pollable_borrow);
		} else {
			errno = EWOULDBLOCK;
			return -1;
		}
	}
}

ssize_t send(int socket, const void *buffer, size_t length, int flags)
{
	return sendto(socket, buffer, length, flags, NULL, 0);
}

ssize_t sendto(int socket, const void *buffer, size_t length, int flags,
	       const struct sockaddr *addr, socklen_t addrlen)
{
	descriptor_table_entry_t *entry;
	if (!descriptor_table_get_ref(socket, &entry)) {
		errno = EBADF;
		return -1;
	}

	if (buffer == NULL) {
		errno = EINVAL;
		return -1;
	}

	switch (entry->tag) {
	case DESCRIPTOR_TABLE_ENTRY_TCP_SOCKET:
		return tcp_sendto(&entry->tcp_socket, buffer, length, flags,
				  addr, addrlen);
	case DESCRIPTOR_TABLE_ENTRY_UDP_SOCKET:
		return udp_sendto(&entry->udp_socket, buffer, length, flags,
				  addr, addrlen);
	default:
		errno = EOPNOTSUPP;
		return -1;
	}
}
