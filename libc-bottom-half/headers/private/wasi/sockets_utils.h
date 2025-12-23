#ifndef __wasi_sockets_utils_h
#define __wasi_sockets_utils_h

#include <netinet/in.h>
#include <wasi/descriptor_table.h>
#include <wasi/api.h>

#ifndef __wasip1__

#if defined(__wasip2__)
typedef network_error_code_t sockets_error_code_t;
typedef network_ip_address_family_t sockets_ip_address_family_t;
typedef network_ip_socket_address_t sockets_ip_socket_address_t;
typedef tcp_own_tcp_socket_t sockets_own_tcp_socket_t;
typedef udp_own_udp_socket_t sockets_own_udp_socket_t;
#elif defined(__wasip3__)
// already defines `sockets_*` types
#else
# error "Unsupported WASI version"
#endif

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

#ifdef __wasip2__
network_borrow_network_t __wasi_sockets_utils__borrow_network();
#endif

int __wasi_sockets_utils__map_error(sockets_error_code_t wasi_error);
bool __wasi_sockets_utils__parse_address(
	sockets_ip_address_family_t expected_family,
	const struct sockaddr *address, socklen_t len,
	sockets_ip_socket_address_t *result, int *error);
bool __wasi_sockets_utils__output_addr_validate(
	sockets_ip_address_family_t expected_family, struct sockaddr *addr,
	socklen_t *addrlen, output_sockaddr_t *result);
void __wasi_sockets_utils__output_addr_write(
	const sockets_ip_socket_address_t input, output_sockaddr_t *output);
int __wasi_sockets_utils__posix_family(sockets_ip_address_family_t wasi_family);
sockets_ip_socket_address_t __wasi_sockets_utils__any_addr(sockets_ip_address_family_t family);
int __wasi_sockets_utils__parse_port(const char *port);
const service_entry_t *__wasi_sockets_utils__get_service_entry_by_name(const char *name);
const service_entry_t *__wasi_sockets_utils__get_service_entry_by_port(const uint16_t port);

#endif // not(__wasip1__)

#endif // __wasi_sockets_utils_h
