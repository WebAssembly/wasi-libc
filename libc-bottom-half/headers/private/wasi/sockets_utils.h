#ifndef __wasi_sockets_utils_h
#define __wasi_sockets_utils_h

#include <netinet/in.h>

#include <wasi/descriptor_table.h>

typedef struct {
	enum {
		OUTPUT_SOCKADDR_NULL,
		OUTPUT_SOCKADDR_V4,
		OUTPUT_SOCKADDR_V6,
	} tag;
	union {
		struct {
			int dummy;
		} null;
		struct {
			struct sockaddr_in *addr;
			socklen_t *addrlen;
		} v4;
		struct {
			struct sockaddr_in6 *addr;
			socklen_t *addrlen;
		} v6;
	};
} output_sockaddr_t;

typedef struct {
	char *s_name;
	uint16_t port;
	uint16_t protocol;
} service_entry_t;

typedef enum {
	SERVICE_PROTOCOL_TCP = 1,
	SERVICE_PROTOCOL_UDP = 2
} service_protocol_e;

network_borrow_network_t __wasi_sockets_utils__borrow_network();
int __wasi_sockets_utils__map_error(network_error_code_t wasi_error);
bool __wasi_sockets_utils__parse_address(
	network_ip_address_family_t expected_family,
	const struct sockaddr *address, socklen_t len,
	network_ip_socket_address_t *result, int *error);
bool __wasi_sockets_utils__output_addr_validate(
	network_ip_address_family_t expected_family, struct sockaddr *addr,
	socklen_t *addrlen, output_sockaddr_t *result);
void __wasi_sockets_utils__output_addr_write(
	const network_ip_socket_address_t input, output_sockaddr_t *output);
int __wasi_sockets_utils__posix_family(network_ip_address_family_t wasi_family);
network_ip_socket_address_t
__wasi_sockets_utils__any_addr(network_ip_address_family_t family);
int __wasi_sockets_utils__tcp_bind(tcp_socket_t *socket,
				   network_ip_socket_address_t *address);
int __wasi_sockets_utils__udp_bind(udp_socket_t *socket,
				   network_ip_socket_address_t *address);
bool __wasi_sockets_utils__stream(udp_socket_t *socket,
				  network_ip_socket_address_t *remote_address,
				  udp_socket_streams_t *result,
				  network_error_code_t *error);
void __wasi_sockets_utils__drop_streams(udp_socket_streams_t streams);
int __wasi_sockets_utils__parse_port(const char *port);
const service_entry_t *__wasi_sockets_utils__get_service_entry_by_name(const char *name);
const service_entry_t *__wasi_sockets_utils__get_service_entry_by_port(const uint16_t port);

// Handles an `error` that came from an operation on `socket`.
//
// If it's "would block" and the socket is in blocking mode, the current thread
// is blocked until the socket is ready. Otherwise errno is set.
//
// Returns 0 if the error was handled and returns -1 otherwise.
static int tcp_socket_handle_error(tcp_socket_t *socket, network_error_code_t error) {
  if (error == NETWORK_ERROR_CODE_WOULD_BLOCK && socket->blocking) {
    poll_own_pollable_t pollable =
      tcp_method_tcp_socket_subscribe(tcp_borrow_tcp_socket(socket->socket));
    poll_method_pollable_block(poll_borrow_pollable(pollable));
    poll_pollable_drop_own(pollable);
    return 0;
  }

  errno = __wasi_sockets_utils__map_error(error);
  return -1;
}

static int udp_socket_handle_error(udp_socket_t *socket, network_error_code_t error) {
  if (error == NETWORK_ERROR_CODE_WOULD_BLOCK && socket->blocking) {
    poll_own_pollable_t pollable =
      udp_method_udp_socket_subscribe(udp_borrow_udp_socket(socket->socket));
    poll_method_pollable_block(poll_borrow_pollable(pollable));
    poll_pollable_drop_own(pollable);
    return 0;
  }

  errno = __wasi_sockets_utils__map_error(error);
  return -1;
}

#endif
