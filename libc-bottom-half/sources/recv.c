#include <sys/socket.h>

#include <errno.h>
#include <stdint.h>

#include <wasi/api.h>
#include <wasi/descriptor_table.h>
#include <wasi/sockets_utils.h>

static ssize_t tcp_recvfrom(tcp_socket_t *socket, uint8_t *buffer,
			    size_t length, int flags, struct sockaddr *addr,
			    socklen_t *addrlen)
{
	// TODO wasi-sockets: flags:
	// - MSG_WAITALL: we can probably support these relatively easy.
	// - MSG_OOB: could be shimmed by always responding that no OOB data is available.
	// - MSG_PEEK: could be shimmed by performing the receive into a local socket-specific buffer. And on subsequent receives first check that buffer.

	const int supported_flags = MSG_DONTWAIT;
	if ((flags & supported_flags) != flags) {
		errno = EOPNOTSUPP;
		return -1;
	}

	if (addr != NULL || addrlen != NULL) {
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

	streams_borrow_input_stream_t rx_borrow =
		streams_borrow_input_stream(connection.input);
	while (true) {
		wasip2_list_u8_t result;
		streams_stream_error_t error;
		if (!streams_method_input_stream_read(rx_borrow, length,
						      &result, &error)) {
			if (error.tag == STREAMS_STREAM_ERROR_CLOSED) {
				return 0;
			} else {
				// TODO wasi-sockets: wasi-sockets has no way to recover TCP stream errors yet.
				errno = EPIPE;
				return -1;
			}
		}

		if (result.len) {
			memcpy(buffer, result.ptr, result.len);
			wasip2_list_u8_free(&result);
			return result.len;
		} else if (should_block) {
			poll_borrow_pollable_t pollable_borrow =
				poll_borrow_pollable(connection.input_pollable);
			poll_method_pollable_block(pollable_borrow);
		} else {
			errno = EWOULDBLOCK;
			return -1;
		}
	}
}

static ssize_t udp_recvfrom(udp_socket_t *socket, uint8_t *buffer,
			    size_t length, int flags, struct sockaddr *addr,
			    socklen_t *addrlen)
{
	// TODO wasi-sockets: flags:
	// - MSG_PEEK: could be shimmed by performing the receive into a local socket-specific buffer. And on subsequent receives first check that buffer.

	const int supported_flags = MSG_DONTWAIT | MSG_TRUNC;
	if ((flags & supported_flags) != flags) {
		errno = EOPNOTSUPP;
		return -1;
	}

	output_sockaddr_t output_addr;
	if (!__wasi_sockets_utils__output_addr_validate(
		    socket->family, addr, addrlen, &output_addr)) {
		errno = EINVAL;
		return -1;
	}

	network_error_code_t error;
	udp_borrow_udp_socket_t socket_borrow =
		udp_borrow_udp_socket(socket->socket);

	udp_socket_streams_t streams;
	switch (socket->state.tag) {
	case UDP_SOCKET_STATE_UNBOUND: {
		// Unlike `send`, `recv` should _not_ perform an implicit bind.
		errno = EINVAL;
		return -1;
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

	bool return_real_size = (flags & MSG_TRUNC) != 0;
	bool should_block = socket->blocking;
	if ((flags & MSG_DONTWAIT) != 0) {
		should_block = false;
	}

	udp_borrow_incoming_datagram_stream_t incoming_borrow =
		udp_borrow_incoming_datagram_stream(streams.incoming);
	while (true) {
		udp_list_incoming_datagram_t datagrams;
		if (!udp_method_incoming_datagram_stream_receive(
			    incoming_borrow, 1, &datagrams, &error)) {
			errno = __wasi_sockets_utils__map_error(error);
			return -1;
		}

		if (datagrams.len) {
			udp_incoming_datagram_t datagram = datagrams.ptr[0];
			size_t datagram_size = datagram.data.len;
			size_t bytes_to_copy =
				datagram_size < length ? datagram_size : length;

			if (output_addr.tag != OUTPUT_SOCKADDR_NULL) {
				__wasi_sockets_utils__output_addr_write(
					datagram.remote_address, &output_addr);
			}

			memcpy(buffer, datagram.data.ptr, bytes_to_copy);
			udp_list_incoming_datagram_free(&datagrams);
			return return_real_size ? datagram_size : bytes_to_copy;

		} else if (should_block) {
			poll_borrow_pollable_t pollable_borrow =
				poll_borrow_pollable(streams.incoming_pollable);
			poll_method_pollable_block(pollable_borrow);
		} else {
			errno = EWOULDBLOCK;
			return -1;
		}
	}
}

ssize_t recv(int socket, void *restrict buffer, size_t length, int flags)
{
	return recvfrom(socket, buffer, length, flags, NULL, NULL);
}

ssize_t recvfrom(int socket, void *__restrict buffer, size_t length, int flags,
		 struct sockaddr *__restrict addr,
		 socklen_t *__restrict addrlen)
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
		return tcp_recvfrom(&entry->tcp_socket, buffer, length, flags,
				    addr, addrlen);
	case DESCRIPTOR_TABLE_ENTRY_UDP_SOCKET:
		return udp_recvfrom(&entry->udp_socket, buffer, length, flags,
				    addr, addrlen);
	default:
		errno = EOPNOTSUPP;
		return -1;
	}
}
