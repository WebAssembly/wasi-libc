#include <errno.h>
#include <netinet/in.h>

#include <descriptor_table.h>
#include "__utils.h"

int tcp_socket(wasi_sockets_0_2_0_rc_2023_10_18_network_ip_address_family_t family, bool blocking)
{
    wasi_sockets_0_2_0_rc_2023_10_18_tcp_create_socket_error_code_t error;
    reactor_own_tcp_socket_t socket;
    if (!wasi_sockets_0_2_0_rc_2023_10_18_tcp_create_socket_create_tcp_socket(family, &socket, &error)) {
        errno = __wasi_sockets_utils__map_error(error);
        return -1;
    }

    reactor_borrow_tcp_socket_t socket_borrow = wasi_sockets_0_2_0_rc_2023_10_18_tcp_borrow_tcp_socket(socket);
    reactor_own_pollable_t socket_pollable = wasi_sockets_0_2_0_rc_2023_10_18_tcp_method_tcp_socket_subscribe(socket_borrow);

    descriptor_table_entry_t entry = {
        .tag = DESCRIPTOR_TABLE_ENTRY_TCP_SOCKET,
        .value = { .tcp_socket = {
            .socket = socket,
            .socket_pollable = socket_pollable,
            .blocking = blocking,
            .state_tag = TCP_SOCKET_STATE_UNBOUND,
            .state = { .unbound = { /* No additional state. */ } },
        } },
    };

    int fd;
    if (!descriptor_table_insert(entry, &fd)) {
        errno = EMFILE;
        return -1;
    }
    return fd;
}

int udp_socket(wasi_sockets_0_2_0_rc_2023_10_18_network_ip_address_family_t family, bool blocking)
{
    // TODO wasi-sockets: implement
    errno = EPROTONOSUPPORT;
    return -1;
}


int socket(int domain, int type, int protocol)
{
    wasi_sockets_0_2_0_rc_2023_10_18_network_ip_address_family_t family;
    switch (domain) {
    case PF_INET:
        family = WASI_SOCKETS_0_2_0_RC_2023_10_18_NETWORK_IP_ADDRESS_FAMILY_IPV4;
        break;

    case PF_INET6:
        family = WASI_SOCKETS_0_2_0_RC_2023_10_18_NETWORK_IP_ADDRESS_FAMILY_IPV6;
        break;

    default:
        errno = EAFNOSUPPORT;
        return -1;
    }

    int real_type = type & ~(SOCK_NONBLOCK | SOCK_CLOEXEC);
    bool blocking = (type & SOCK_NONBLOCK) == 0;
    // Ignore SOCK_CLOEXEC flag. That concept does not exist in WASI.

    if (real_type == SOCK_STREAM && (protocol == 0 || protocol == IPPROTO_TCP)) {
        return tcp_socket(family, blocking);

    } else if (real_type == SOCK_DGRAM && (protocol == 0 || protocol == IPPROTO_UDP)) {
        return udp_socket(family, blocking);
        
    } else {
        errno = EPROTONOSUPPORT;
        return -1;
    }
}
