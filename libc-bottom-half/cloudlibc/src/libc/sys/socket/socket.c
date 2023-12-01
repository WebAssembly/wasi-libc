#include <errno.h>
#include <netinet/in.h>

#include <descriptor_table.h>

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

    switch (type & ~(SOCK_NONBLOCK | SOCK_CLOEXEC)) {
    case SOCK_STREAM: {
        wasi_sockets_0_2_0_rc_2023_10_18_tcp_create_socket_error_code_t error;
        reactor_own_tcp_socket_t socket;
        if (wasi_sockets_0_2_0_rc_2023_10_18_tcp_create_socket_create_tcp_socket(
                family, &socket, &error)) {
            descriptor_table_variant_t variant = { .tag = DESCRIPTOR_TABLE_VARIANT_TCP_NEW,
                .value = { .tcp_new = { .socket = socket, .blocking = (type & SOCK_NONBLOCK) == 0 } } };
            int fd;
            if (!descriptor_table_insert(variant, &fd)) {
                errno = ENOMEM;
                return -1;
            }
            return fd;
        } else {
            // TODO: map errors appropriately
            errno = EBADF;
            return -1;
        }
    }

    case SOCK_DGRAM:
        // TODO
        errno = EPROTONOSUPPORT;
        return -1;

    default:
        errno = EPROTONOSUPPORT;
        return -1;
    }
}

int connect(int fd, const struct sockaddr* address, socklen_t len)
{
    descriptor_table_variant_t variant;
    if (!descriptor_table_get(fd, &variant)) {
        errno = EBADF;
        return -1;
    }

    descriptor_table_tcp_new_t socket;
    switch (variant.tag) {
    case DESCRIPTOR_TABLE_VARIANT_TCP_NEW:
        socket = variant.value.tcp_new;
        break;

    case DESCRIPTOR_TABLE_VARIANT_TCP_CONNECTED:
        // TODO: should we disallow re-connecting an already-connected socket?
        socket = variant.value.tcp_connected.socket;
        break;

    default:
        errno = EBADF;
        return -1;
    }

    reactor_borrow_tcp_socket_t socket_borrow = wasi_sockets_0_2_0_rc_2023_10_18_tcp_borrow_tcp_socket(socket.socket);

    wasi_sockets_0_2_0_rc_2023_10_18_network_ip_socket_address_t ip_address;
    switch (address->sa_family) {
    case AF_INET: {
        struct sockaddr_in* ipv4 = (struct sockaddr_in*)address;
        unsigned ip = ipv4->sin_addr.s_addr;
        unsigned short port = ipv4->sin_port;
        wasi_sockets_0_2_0_rc_2023_10_18_network_ip_socket_address_t my_ip_address = {
            .tag = WASI_SOCKETS_0_2_0_RC_2023_10_18_NETWORK_IP_SOCKET_ADDRESS_IPV4,
            .val = { .ipv4 = { .port = (port << 8) | (port >> 8),
                         .address = { ip & 0xFF, (ip >> 8) & 0xFF, (ip >> 16) & 0xFF, ip >> 24 } } }
        };
        ip_address = my_ip_address;
        break;
    }

    case AF_INET6: {
        struct sockaddr_in6* ipv6 = (struct sockaddr_in6*)address;
        unsigned char* ip = (unsigned char*)&(ipv6->sin6_addr.s6_addr);
        unsigned short port = ipv6->sin6_port;
        wasi_sockets_0_2_0_rc_2023_10_18_network_ip_socket_address_t my_ip_address = {
            .tag = WASI_SOCKETS_0_2_0_RC_2023_10_18_NETWORK_IP_SOCKET_ADDRESS_IPV6,
            .val = { .ipv6 = { .port = (port << 8) | (port >> 8),
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
                         // TODO: do these need to be endian-reversed?
                         .flow_info = ipv6->sin6_flowinfo,
                         .scope_id = ipv6->sin6_flowinfo } }
        };
        ip_address = my_ip_address;
        break;
    }

    default:
        errno = EPROTONOSUPPORT;
        return -1;
    }

    reactor_own_network_t network = wasi_sockets_0_2_0_rc_2023_10_18_instance_network_instance_network();
    wasi_sockets_0_2_0_rc_2023_10_18_tcp_error_code_t error;
    reactor_borrow_network_t network_borrow = wasi_sockets_0_2_0_rc_2023_10_18_network_borrow_network(network);
    bool result = wasi_sockets_0_2_0_rc_2023_10_18_tcp_method_tcp_socket_start_connect(socket_borrow, network_borrow, &ip_address, &error);
    wasi_sockets_0_2_0_rc_2023_10_18_network_network_drop_own(network);

    if (!result) {
        // TODO: map errors appropriately
        errno = EBADF;
        return -1;
    }

    reactor_tuple2_own_input_stream_own_output_stream_t rx_tx;
    while (!wasi_sockets_0_2_0_rc_2023_10_18_tcp_method_tcp_socket_finish_connect(socket_borrow, &rx_tx, &error)) {
        if (error == WASI_SOCKETS_0_2_0_RC_2023_10_18_NETWORK_ERROR_CODE_WOULD_BLOCK) {
            if (socket.blocking) {
                reactor_own_pollable_t pollable = wasi_sockets_0_2_0_rc_2023_10_18_tcp_method_tcp_socket_subscribe(socket_borrow);
                reactor_borrow_pollable_t pollable_borrow = wasi_io_0_2_0_rc_2023_10_18_poll_borrow_pollable(pollable);
                wasi_io_0_2_0_rc_2023_10_18_poll_poll_one(pollable_borrow);
                wasi_io_0_2_0_rc_2023_10_18_poll_pollable_drop_own(pollable);
            } else {
                descriptor_table_variant_t new_variant = { .tag = DESCRIPTOR_TABLE_VARIANT_TCP_CONNECTING,
                    .value = { .tcp_new = socket } };
                if (!descriptor_table_update(fd, new_variant)) {
                    abort();
                }
                errno = EINPROGRESS;
                return -1;
            }
        } else {
            // TODO: map errors appropriately
            errno = EBADF;
            return -1;
        }
    }

    descriptor_table_variant_t new_variant = { .tag = DESCRIPTOR_TABLE_VARIANT_TCP_CONNECTED,
        .value = { .tcp_connected = { .socket = socket, .rx = rx_tx.f0, .tx = rx_tx.f1 } } };
    if (!descriptor_table_update(fd, new_variant)) {
        abort();
    }

    return 0;
}
