#include <errno.h>

#include <wasi/sockets_utils.h>

static network_own_network_t global_network;
static bool global_network_initialized = false;

network_borrow_network_t __wasi_sockets_utils__borrow_network()
{
	if (!global_network_initialized) {
		global_network = instance_network_instance_network();
		global_network_initialized = true;
	}

	return network_borrow_network(global_network);
}

int __wasi_sockets_utils__map_error(network_error_code_t wasi_error)
{
	switch (wasi_error) {
	case NETWORK_ERROR_CODE_ACCESS_DENIED:
		return EACCES;
	case NETWORK_ERROR_CODE_NOT_SUPPORTED:
		return EOPNOTSUPP;
	case NETWORK_ERROR_CODE_INVALID_ARGUMENT:
		return EINVAL;
	case NETWORK_ERROR_CODE_OUT_OF_MEMORY:
		return ENOMEM;
	case NETWORK_ERROR_CODE_TIMEOUT:
		return ETIMEDOUT;
	case NETWORK_ERROR_CODE_CONCURRENCY_CONFLICT:
		return EALREADY;
	case NETWORK_ERROR_CODE_WOULD_BLOCK:
		return EWOULDBLOCK;
	case NETWORK_ERROR_CODE_NEW_SOCKET_LIMIT:
		return EMFILE;
	case NETWORK_ERROR_CODE_ADDRESS_NOT_BINDABLE:
		return EADDRNOTAVAIL;
	case NETWORK_ERROR_CODE_ADDRESS_IN_USE:
		return EADDRINUSE;
	case NETWORK_ERROR_CODE_REMOTE_UNREACHABLE:
		return EHOSTUNREACH;
	case NETWORK_ERROR_CODE_CONNECTION_REFUSED:
		return ECONNREFUSED;
	case NETWORK_ERROR_CODE_CONNECTION_RESET:
		return ECONNRESET;
	case NETWORK_ERROR_CODE_CONNECTION_ABORTED:
		return ECONNABORTED;
	case NETWORK_ERROR_CODE_DATAGRAM_TOO_LARGE:
		return EMSGSIZE;

	case NETWORK_ERROR_CODE_INVALID_STATE:
	case NETWORK_ERROR_CODE_NOT_IN_PROGRESS:
		abort(); // If our internal state checks are working right, these errors should never show up.
		break;

	case NETWORK_ERROR_CODE_NAME_UNRESOLVABLE:
	case NETWORK_ERROR_CODE_TEMPORARY_RESOLVER_FAILURE:
	case NETWORK_ERROR_CODE_PERMANENT_RESOLVER_FAILURE:
		abort(); // These errors are specific to getaddrinfo, which should have filtered these errors out before calling this generic method
		break;

	case NETWORK_ERROR_CODE_UNKNOWN:
	default:
		return EOPNOTSUPP;
	}
}

bool __wasi_sockets_utils__parse_address(
	network_ip_address_family_t expected_family,
	const struct sockaddr *address, socklen_t len,
	network_ip_socket_address_t *result, int *error)
{
	if (address == NULL || len < sizeof(struct sockaddr)) {
		*error = EINVAL;
		return false;
	}

	switch (expected_family) {
	case NETWORK_IP_ADDRESS_FAMILY_IPV4: {
		if (address->sa_family != AF_INET) {
			*error = EAFNOSUPPORT;
			return false;
		}

		if (len < sizeof(struct sockaddr_in)) {
			*error = EINVAL;
			return false;
		}

		struct sockaddr_in *ipv4 = (struct sockaddr_in *)address;
		unsigned ip = ipv4->sin_addr.s_addr;
		unsigned short port = ipv4->sin_port;
		*result = (network_ip_socket_address_t){
            .tag = NETWORK_IP_SOCKET_ADDRESS_IPV4,
            .val = { .ipv4 = {
                .port = ntohs(port), // (port << 8) | (port >> 8),
                .address = { ip & 0xFF, (ip >> 8) & 0xFF, (ip >> 16) & 0xFF, ip >> 24 },
            } },
        };
		return true;
	}
	case NETWORK_IP_ADDRESS_FAMILY_IPV6: {
		if (address->sa_family != AF_INET6) {
			*error = EAFNOSUPPORT;
			return false;
		}

		if (len < sizeof(struct sockaddr_in6)) {
			*error = EINVAL;
			return false;
		}

		struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)address;
		unsigned char *ip = (unsigned char *)&(ipv6->sin6_addr.s6_addr);
		unsigned short port = ipv6->sin6_port;
		*result = (network_ip_socket_address_t){
            .tag = NETWORK_IP_SOCKET_ADDRESS_IPV6,
            .val = { .ipv6 = {
                .port = ntohs(port),
                .address = {
                    (((unsigned short)ip[0]) << 8) | ip[1],
                    (((unsigned short)ip[2]) << 8) | ip[3],
                    (((unsigned short)ip[4]) << 8) | ip[5],
                    (((unsigned short)ip[6]) << 8) | ip[7],
                    (((unsigned short)ip[8]) << 8) | ip[9],
                    (((unsigned short)ip[10]) << 8) | ip[11],
                    (((unsigned short)ip[12]) << 8) | ip[13],
                    (((unsigned short)ip[14]) << 8) | ip[15],
                },
                // TODO wasi-sockets: do these need to be endian-reversed?
                .flow_info = ipv6->sin6_flowinfo,
                .scope_id = ipv6->sin6_scope_id,
            } }
        };
		return true;
	}
	default: /* unreachable */
		abort();
	}
}

bool __wasi_sockets_utils__output_addr_validate(
	network_ip_address_family_t expected_family, struct sockaddr *addr,
	socklen_t *addrlen, output_sockaddr_t *result)
{
	// The address parameters must be either both null or both _not_ null.

	if (addr == NULL && addrlen == NULL) {
		*result = (output_sockaddr_t){ .tag = OUTPUT_SOCKADDR_NULL,
					       .null = {} };
		return true;

	} else if (addr != NULL && addrlen != NULL) {
		if (expected_family == NETWORK_IP_ADDRESS_FAMILY_IPV4) {
			if (*addrlen < sizeof(struct sockaddr_in)) {
				return false;
			}

			*result =
				(output_sockaddr_t){ .tag = OUTPUT_SOCKADDR_V4,
						     .v4 = {
							     .addr = (struct sockaddr_in
									      *)
								     addr,
							     .addrlen = addrlen,
						     } };
			return true;

		} else if (expected_family == NETWORK_IP_ADDRESS_FAMILY_IPV6) {
			if (*addrlen < sizeof(struct sockaddr_in6)) {
				return false;
			}

			*result =
				(output_sockaddr_t){ .tag = OUTPUT_SOCKADDR_V6,
						     .v6 = {
							     .addr = (struct sockaddr_in6
									      *)
								     addr,
							     .addrlen = addrlen,
						     } };
			return true;

		} else {
			abort();
		}

	} else {
		return false;
	}
}

void __wasi_sockets_utils__output_addr_write(
	const network_ip_socket_address_t input, output_sockaddr_t *output)
{
	switch (input.tag) {
	case NETWORK_IP_SOCKET_ADDRESS_IPV4: {
		if (output->tag != OUTPUT_SOCKADDR_V4) {
			abort();
		}

		network_ipv4_socket_address_t input_v4 = input.val.ipv4;
		network_ipv4_address_t ip = input_v4.address;

		*output->v4.addrlen = sizeof(struct sockaddr_in);
		*output->v4.addr = (struct sockaddr_in){
			.sin_family = AF_INET,
			.sin_port = htons(input_v4.port),
			.sin_addr = { .s_addr = ip.f0 | (ip.f1 << 8) |
						(ip.f2 << 16) | (ip.f3 << 24) },
		};
		return;
	}
	case NETWORK_IP_SOCKET_ADDRESS_IPV6: {
		if (output->tag != OUTPUT_SOCKADDR_V6) {
			abort();
		}

		network_ipv6_socket_address_t input_v6 = input.val.ipv6;
		network_ipv6_address_t ip = input_v6.address;

		*output->v6.addrlen = sizeof(struct sockaddr_in6);
		*output->v6.addr = (struct sockaddr_in6) {
            .sin6_family = AF_INET6,
            .sin6_port = htons(input_v6.port),
            .sin6_addr = { .s6_addr = {
                ip.f0 >> 8, ip.f0 & 0xFF,
                ip.f1 >> 8, ip.f1 & 0xFF,
                ip.f2 >> 8, ip.f2 & 0xFF,
                ip.f3 >> 8, ip.f3 & 0xFF,
                ip.f4 >> 8, ip.f4 & 0xFF,
                ip.f5 >> 8, ip.f5 & 0xFF,
                ip.f6 >> 8, ip.f6 & 0xFF,
                ip.f7 >> 8, ip.f7 & 0xFF,
            } },
            // TODO wasi-sockets: do these need to be endian-reversed?
            .sin6_flowinfo = input_v6.flow_info,
            .sin6_scope_id = input_v6.scope_id,
        };
		return;
	}
	default: /* unreachable */
		abort();
	}
}

int __wasi_sockets_utils__posix_family(network_ip_address_family_t wasi_family)
{
	switch (wasi_family) {
	case NETWORK_IP_ADDRESS_FAMILY_IPV4:
		return AF_INET;
	case NETWORK_IP_ADDRESS_FAMILY_IPV6:
		return AF_INET6;
	default: /* unreachable */
		abort();
	}
}

network_ip_socket_address_t
__wasi_sockets_utils__any_addr(network_ip_address_family_t family)
{
	switch (family) {
	case NETWORK_IP_ADDRESS_FAMILY_IPV4:
		return (network_ip_socket_address_t){ .tag = NETWORK_IP_SOCKET_ADDRESS_IPV4,
						      .val = {
							      .ipv4 = {
								      .port = 0,
								      .address = { 0,
										   0,
										   0,
										   0 },
							      } } };
	case NETWORK_IP_ADDRESS_FAMILY_IPV6:
		return (network_ip_socket_address_t){ .tag = NETWORK_IP_SOCKET_ADDRESS_IPV6,
						      .val = {
							      .ipv6 = {
								      .port = 0,
								      .address = { 0,
										   0,
										   0,
										   0,
										   0,
										   0,
										   0,
										   0 },
								      .flow_info =
									      0,
								      .scope_id =
									      0,
							      } } };
	default: /* unreachable */
		abort();
	}
}

int __wasi_sockets_utils__tcp_bind(tcp_socket_t *socket,
				   network_ip_socket_address_t *address)
{
	tcp_socket_state_unbound_t unbound;
	if (socket->state.tag == TCP_SOCKET_STATE_UNBOUND) {
		unbound = socket->state.unbound;
	} else {
		errno = EINVAL;
		return -1;
	}

	network_error_code_t error;
	network_borrow_network_t network_borrow =
		__wasi_sockets_utils__borrow_network();
	tcp_borrow_tcp_socket_t socket_borrow =
		tcp_borrow_tcp_socket(socket->socket);

	if (!tcp_method_tcp_socket_start_bind(socket_borrow, network_borrow,
					      address, &error)) {
		errno = __wasi_sockets_utils__map_error(error);
		return -1;
	}

	// Bind has successfully started. Attempt to finish it:
	while (!tcp_method_tcp_socket_finish_bind(socket_borrow, &error)) {
		if (error == NETWORK_ERROR_CODE_WOULD_BLOCK) {
			poll_borrow_pollable_t pollable_borrow =
				poll_borrow_pollable(socket->socket_pollable);
			poll_method_pollable_block(pollable_borrow);
		} else {
			errno = __wasi_sockets_utils__map_error(error);
			return -1;
		}
	}

	// Bind successful.

	socket->state =
		(tcp_socket_state_t){ .tag = TCP_SOCKET_STATE_BOUND,
				      .bound = { /* No additional state */ } };
	return 0;
}

int __wasi_sockets_utils__udp_bind(udp_socket_t *socket,
				   network_ip_socket_address_t *address)
{
	udp_socket_state_unbound_t unbound;
	if (socket->state.tag == UDP_SOCKET_STATE_UNBOUND) {
		unbound = socket->state.unbound;
	} else {
		errno = EINVAL;
		return -1;
	}

	network_error_code_t error;
	network_borrow_network_t network_borrow =
		__wasi_sockets_utils__borrow_network();
	udp_borrow_udp_socket_t socket_borrow =
		udp_borrow_udp_socket(socket->socket);

	if (!udp_method_udp_socket_start_bind(socket_borrow, network_borrow,
					      address, &error)) {
		errno = __wasi_sockets_utils__map_error(error);
		return -1;
	}

	// Bind has successfully started. Attempt to finish it:
	while (!udp_method_udp_socket_finish_bind(socket_borrow, &error)) {
		if (error == NETWORK_ERROR_CODE_WOULD_BLOCK) {
			poll_borrow_pollable_t pollable_borrow =
				poll_borrow_pollable(socket->socket_pollable);
			poll_method_pollable_block(pollable_borrow);
		} else {
			errno = __wasi_sockets_utils__map_error(error);
			return -1;
		}
	}

	// Bind successful.

	socket->state =
		(udp_socket_state_t){ .tag = UDP_SOCKET_STATE_BOUND_NOSTREAMS,
				      .bound_nostreams = {} };
	return 0;
}

bool __wasi_sockets_utils__create_streams(
	udp_borrow_udp_socket_t socket_borrow,
	network_ip_socket_address_t *remote_address,
	udp_socket_streams_t *result, network_error_code_t *error)
{
	udp_tuple2_own_incoming_datagram_stream_own_outgoing_datagram_stream_t
		io;
	if (!udp_method_udp_socket_stream(socket_borrow, remote_address, &io,
					  error)) {
		return false;
	}

	udp_own_incoming_datagram_stream_t incoming = io.f0;
	udp_borrow_incoming_datagram_stream_t incoming_borrow =
		udp_borrow_incoming_datagram_stream(incoming);
	poll_own_pollable_t incoming_pollable =
		udp_method_incoming_datagram_stream_subscribe(incoming_borrow);

	udp_own_outgoing_datagram_stream_t outgoing = io.f1;
	udp_borrow_outgoing_datagram_stream_t outgoing_borrow =
		udp_borrow_outgoing_datagram_stream(outgoing);
	poll_own_pollable_t outgoing_pollable =
		udp_method_outgoing_datagram_stream_subscribe(outgoing_borrow);

	*result = (udp_socket_streams_t){
		.incoming = incoming,
		.incoming_pollable = incoming_pollable,
		.outgoing = outgoing,
		.outgoing_pollable = outgoing_pollable,
	};
	return true;
}

void __wasi_sockets_utils__drop_streams(udp_socket_streams_t streams)
{
	poll_pollable_drop_own(streams.incoming_pollable);
	poll_pollable_drop_own(streams.outgoing_pollable);
	udp_incoming_datagram_stream_drop_own(streams.incoming);
	udp_outgoing_datagram_stream_drop_own(streams.outgoing);
}

bool __wasi_sockets_utils__stream(
	udp_socket_t *socket,
	network_ip_socket_address_t
		*remote_address, // May be null to "disconnect"
	udp_socket_streams_t *result, network_error_code_t *error)
{
	// Assert that:
	// - We're already bound. This is required by WASI.
	// - We have no active streams. From WASI:
	//   > Implementations may trap if the streams returned by a previous
	//   > invocation haven't been dropped yet before calling `stream` again.
	if (socket->state.tag != UDP_SOCKET_STATE_BOUND_NOSTREAMS) {
		abort();
	}

	udp_borrow_udp_socket_t socket_borrow =
		udp_borrow_udp_socket(socket->socket);

	if (!__wasi_sockets_utils__create_streams(socket_borrow, remote_address,
						  result, error)) {
		return false;
	}

	if (remote_address != NULL) {
		socket->state =
			(udp_socket_state_t){ .tag = UDP_SOCKET_STATE_CONNECTED,
					      .connected = {
						      .streams = *result,
					      } };
	} else {
		socket->state =
			(udp_socket_state_t){ .tag = UDP_SOCKET_STATE_BOUND_STREAMING,
					      .bound_streaming = {
						      .streams = *result,
					      } };
	}

	return true;
}
