#include <errno.h>
#include <netinet/in.h>

#include <descriptor_table.h>
#include "__utils.h"

bool tcp_getsockname(tcp_socket_t* socket, network_ip_socket_address_t* out_address)
{
    network_error_code_t error;
    tcp_borrow_tcp_socket_t socket_borrow = tcp_borrow_tcp_socket(socket->socket);
    if (!tcp_method_tcp_socket_local_address(socket_borrow, out_address, &error)) {
        errno = __wasi_sockets_utils__map_error(error);
        return false;
    }

    return true;
}

bool tcp_getpeername(tcp_socket_t* socket, network_ip_socket_address_t* out_address)
{
    network_error_code_t error;
    tcp_borrow_tcp_socket_t socket_borrow = tcp_borrow_tcp_socket(socket->socket);
    if (!tcp_method_tcp_socket_remote_address(socket_borrow, out_address, &error)) {
        errno = __wasi_sockets_utils__map_error(error);
        return false;
    }

    return true;
}

bool udp_getsockname(udp_socket_t* socket, network_ip_socket_address_t* out_address)
{
    // TODO wasi-sockets: implement
    errno = EOPNOTSUPP;
    return -1;
}

bool udp_getpeername(udp_socket_t* socket, network_ip_socket_address_t* out_address)
{
    // TODO wasi-sockets: implement
    errno = EOPNOTSUPP;
    return -1;
}

int getsockname(int socket, struct sockaddr *__restrict addr, socklen_t *__restrict addrlen)
{
    descriptor_table_entry_t* entry;
    if (!descriptor_table_get_ref(socket, &entry)) {
        errno = EBADF;
        return -1;
    }

    if (addr == NULL || addrlen == NULL || *addrlen == 0) {
        errno = EINVAL;
        return -1;
    }

    network_ip_socket_address_t out_address;

    switch (entry->tag)
    {
    case DESCRIPTOR_TABLE_ENTRY_TCP_SOCKET:
        if (!tcp_getsockname(&entry->value.tcp_socket, &out_address)) {
            // errno is set by tcp_getsockname
            return -1;
        }
        break;
    case DESCRIPTOR_TABLE_ENTRY_UDP_SOCKET:
        if (!udp_getsockname(&entry->value.udp_socket, &out_address)) {
            // errno is set by udp_getsockname
            return -1;
        }
        break;
    default:
        errno = EOPNOTSUPP;
        return -1;
    }

    int err;
    if (!__wasi_sockets_utils__format_address(&out_address, addr, addrlen, &err)) {
        errno = err;
        return -1;
    }

    return 0;
}

int getpeername(int socket, struct sockaddr *__restrict addr, socklen_t *__restrict addrlen)
{
    descriptor_table_entry_t* entry;
    if (!descriptor_table_get_ref(socket, &entry)) {
        errno = EBADF;
        return -1;
    }

    if (addr == NULL || addrlen == NULL || *addrlen == 0) {
        errno = EINVAL;
        return -1;
    }

    network_ip_socket_address_t out_address;

    switch (entry->tag)
    {
    case DESCRIPTOR_TABLE_ENTRY_TCP_SOCKET:
        if (!tcp_getpeername(&entry->value.tcp_socket, &out_address)) {
            // errno is set by tcp_getpeername
            return -1;
        }
        break;
    case DESCRIPTOR_TABLE_ENTRY_UDP_SOCKET:
        if (!udp_getpeername(&entry->value.udp_socket, &out_address)) {
            // errno is set by udp_getpeername
            return -1;
        }
        break;
    default:
        errno = EOPNOTSUPP;
        return -1;
    }

    int err;
    if (!__wasi_sockets_utils__format_address(&out_address, addr, addrlen, &err)) {
        errno = err;
        return -1;
    }
    
    return 0;
}
