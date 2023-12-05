#include <errno.h>

#include "__utils.h"

static reactor_own_network_t global_network;
static bool global_network_initialized = false;

reactor_borrow_network_t __wasi_sockets_utils__borrow_network() {
    if (!global_network_initialized) {
        global_network = wasi_sockets_0_2_0_rc_2023_10_18_instance_network_instance_network();
        global_network_initialized = true;
    }

    return wasi_sockets_0_2_0_rc_2023_10_18_network_borrow_network(global_network);
}

int __wasi_sockets_utils__map_error(wasi_sockets_0_2_0_rc_2023_10_18_network_error_code_t wasi_error) {
    switch (wasi_error)
    {
    case WASI_SOCKETS_0_2_0_RC_2023_10_18_NETWORK_ERROR_CODE_ACCESS_DENIED: return EACCES;
    case WASI_SOCKETS_0_2_0_RC_2023_10_18_NETWORK_ERROR_CODE_NOT_SUPPORTED: return EOPNOTSUPP;
    case WASI_SOCKETS_0_2_0_RC_2023_10_18_NETWORK_ERROR_CODE_INVALID_ARGUMENT: return EINVAL;
    case WASI_SOCKETS_0_2_0_RC_2023_10_18_NETWORK_ERROR_CODE_OUT_OF_MEMORY: return ENOMEM;
    case WASI_SOCKETS_0_2_0_RC_2023_10_18_NETWORK_ERROR_CODE_TIMEOUT: return ETIMEDOUT;
    case WASI_SOCKETS_0_2_0_RC_2023_10_18_NETWORK_ERROR_CODE_CONCURRENCY_CONFLICT: return EALREADY;
    case WASI_SOCKETS_0_2_0_RC_2023_10_18_NETWORK_ERROR_CODE_WOULD_BLOCK: return EWOULDBLOCK;
    case WASI_SOCKETS_0_2_0_RC_2023_10_18_NETWORK_ERROR_CODE_NEW_SOCKET_LIMIT: return EMFILE;
    case WASI_SOCKETS_0_2_0_RC_2023_10_18_NETWORK_ERROR_CODE_ADDRESS_NOT_BINDABLE: return EADDRNOTAVAIL;
    case WASI_SOCKETS_0_2_0_RC_2023_10_18_NETWORK_ERROR_CODE_ADDRESS_IN_USE: return EADDRINUSE;
    case WASI_SOCKETS_0_2_0_RC_2023_10_18_NETWORK_ERROR_CODE_REMOTE_UNREACHABLE: return EHOSTUNREACH;
    case WASI_SOCKETS_0_2_0_RC_2023_10_18_NETWORK_ERROR_CODE_CONNECTION_REFUSED: return ECONNREFUSED;
    case WASI_SOCKETS_0_2_0_RC_2023_10_18_NETWORK_ERROR_CODE_CONNECTION_RESET: return ECONNRESET;
    case WASI_SOCKETS_0_2_0_RC_2023_10_18_NETWORK_ERROR_CODE_CONNECTION_ABORTED: return ECONNABORTED;
    case WASI_SOCKETS_0_2_0_RC_2023_10_18_NETWORK_ERROR_CODE_DATAGRAM_TOO_LARGE: return EMSGSIZE;

    case WASI_SOCKETS_0_2_0_RC_2023_10_18_NETWORK_ERROR_CODE_INVALID_STATE:
    case WASI_SOCKETS_0_2_0_RC_2023_10_18_NETWORK_ERROR_CODE_NOT_IN_PROGRESS:
        abort(); // If our internal state checks are working right, these errors should never show up.
        break;

    case WASI_SOCKETS_0_2_0_RC_2023_10_18_NETWORK_ERROR_CODE_NAME_UNRESOLVABLE:
    case WASI_SOCKETS_0_2_0_RC_2023_10_18_NETWORK_ERROR_CODE_TEMPORARY_RESOLVER_FAILURE:
    case WASI_SOCKETS_0_2_0_RC_2023_10_18_NETWORK_ERROR_CODE_PERMANENT_RESOLVER_FAILURE:
        abort(); // These errors are specific to getaddrinfo, which should have filtered these errors out before calling this generic method
        break;

    case WASI_SOCKETS_0_2_0_RC_2023_10_18_NETWORK_ERROR_CODE_UNKNOWN:
    default:
        return EOPNOTSUPP;
    }
}

bool __wasi_sockets_utils__parse_address(const struct sockaddr* address, socklen_t len, wasi_sockets_0_2_0_rc_2023_10_18_network_ip_socket_address_t* output, int* error)
{
    *error = 0;

    socklen_t smallest_sockaddr_size = sizeof(struct sockaddr);
    if (address == NULL || len < smallest_sockaddr_size) {
        *error = EINVAL;
        return false;
    }

    switch (address->sa_family) {
    case AF_INET: {
        if (len < sizeof(struct sockaddr_in)) {
            *error = EINVAL;
            return false;
        }

        struct sockaddr_in* ipv4 = (struct sockaddr_in*)address;
        unsigned ip = ipv4->sin_addr.s_addr;
        unsigned short port = ipv4->sin_port;
        *output = (wasi_sockets_0_2_0_rc_2023_10_18_network_ip_socket_address_t){
            .tag = WASI_SOCKETS_0_2_0_RC_2023_10_18_NETWORK_IP_SOCKET_ADDRESS_IPV4,
            .val = { .ipv4 = {
                .port = ntohs(port), // (port << 8) | (port >> 8),
                .address = { ip & 0xFF, (ip >> 8) & 0xFF, (ip >> 16) & 0xFF, ip >> 24 },
            } },
        };
        return true;
    }
    case AF_INET6: {
        if (len < sizeof(struct sockaddr_in6)) {
            *error = EINVAL;
            return false;
        }

        struct sockaddr_in6* ipv6 = (struct sockaddr_in6*)address;
        unsigned char* ip = (unsigned char*)&(ipv6->sin6_addr.s6_addr);
        unsigned short port = ipv6->sin6_port;
        *output = (wasi_sockets_0_2_0_rc_2023_10_18_network_ip_socket_address_t){
            .tag = WASI_SOCKETS_0_2_0_RC_2023_10_18_NETWORK_IP_SOCKET_ADDRESS_IPV6,
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
    default:
        *error = EAFNOSUPPORT;
        return false;
    }
}

bool __wasi_sockets_utils__format_address(const wasi_sockets_0_2_0_rc_2023_10_18_network_ip_socket_address_t* address, struct sockaddr* output_addr, socklen_t* output_addrlen, int* error) {
    *error = 0;

    if (output_addr == NULL || output_addrlen == NULL) {
        *error = EINVAL;
        return false;
    }

    switch (address->tag)
    {
    case WASI_SOCKETS_0_2_0_RC_2023_10_18_NETWORK_IP_SOCKET_ADDRESS_IPV4: {
        if (*output_addrlen < sizeof(struct sockaddr_in)) {
            *error = EINVAL;
            return false;
        }
        *output_addrlen = sizeof(struct sockaddr_in);
        struct sockaddr_in* ipv4 = (struct sockaddr_in*)output_addr;

        wasi_sockets_0_2_0_rc_2023_10_18_network_ipv4_address_t ip = address->val.ipv4.address;

        *ipv4 = (struct sockaddr_in) {
            .sin_family = AF_INET,
            .sin_port = htons(address->val.ipv4.port),
            .sin_addr = { .s_addr = ip.f0 | (ip.f1 << 8) | (ip.f2 << 16) | (ip.f3 << 24) },
        };
        return true;
    }
    case WASI_SOCKETS_0_2_0_RC_2023_10_18_NETWORK_IP_SOCKET_ADDRESS_IPV6: {
        if (*output_addrlen < sizeof(struct sockaddr_in6)) {
            *error = EINVAL;
            return false;
        }
        *output_addrlen = sizeof(struct sockaddr_in6);
        struct sockaddr_in6* ipv6 = (struct sockaddr_in6*)output_addr;

        wasi_sockets_0_2_0_rc_2023_10_18_network_ipv6_address_t ip = address->val.ipv6.address;

        *ipv6 = (struct sockaddr_in6) {
            .sin6_family = AF_INET,
            .sin6_port = htons(address->val.ipv6.port),
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
            .sin6_flowinfo = address->val.ipv6.flow_info,
            .sin6_scope_id = address->val.ipv6.scope_id,
        };
        return true;
    }
    default: /* unreachable */ abort();
    }
}

int __wasi_sockets_utils__posix_family(wasi_sockets_0_2_0_rc_2023_10_18_network_ip_address_family_t wasi_family) {
    switch (wasi_family)
    {
    case WASI_SOCKETS_0_2_0_RC_2023_10_18_NETWORK_IP_ADDRESS_FAMILY_IPV4: return AF_INET;
    case WASI_SOCKETS_0_2_0_RC_2023_10_18_NETWORK_IP_ADDRESS_FAMILY_IPV6: return AF_INET6;
    default: /* unreachable */ abort();
    }
}

wasi_sockets_0_2_0_rc_2023_10_18_network_ip_socket_address_t __wasi_sockets_utils__any_addr(wasi_sockets_0_2_0_rc_2023_10_18_network_ip_address_family_t family) {
    switch (family)
    {
    case WASI_SOCKETS_0_2_0_RC_2023_10_18_NETWORK_IP_ADDRESS_FAMILY_IPV4:
        return (wasi_sockets_0_2_0_rc_2023_10_18_network_ip_socket_address_t){
            .tag = WASI_SOCKETS_0_2_0_RC_2023_10_18_NETWORK_IP_SOCKET_ADDRESS_IPV4,
            .val = { .ipv4 = {
                .port = 0,
                .address = { 0, 0, 0, 0 },
            }}
        };
    case WASI_SOCKETS_0_2_0_RC_2023_10_18_NETWORK_IP_ADDRESS_FAMILY_IPV6:
        return (wasi_sockets_0_2_0_rc_2023_10_18_network_ip_socket_address_t){
            .tag = WASI_SOCKETS_0_2_0_RC_2023_10_18_NETWORK_IP_SOCKET_ADDRESS_IPV6,
            .val = { .ipv6 = {
                .port = 0,
                .address = { 0, 0, 0, 0, 0, 0, 0, 0 },
                .flow_info = 0,
                .scope_id = 0,
            }}
        };
    default: /* unreachable */ abort();
    }
}

int __wasi_sockets_utils__tcp_bind(tcp_socket_t* socket, wasi_sockets_0_2_0_rc_2023_10_18_network_ip_socket_address_t* address) {

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