// Copyright (c) 2015-2016 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <sys/socket.h>

#include <assert.h>
#include <wasi/api.h>
#include <errno.h>

#include <descriptor_table.h>
#include "__utils.h"

static_assert(SHUT_RD == __WASI_SDFLAGS_RD, "Value mismatch");
static_assert(SHUT_WR == __WASI_SDFLAGS_WR, "Value mismatch");

int tcp_shutdown(tcp_socket_t* socket, int posix_how)
{
    wasi_sockets_0_2_0_rc_2023_10_18_tcp_shutdown_type_t wasi_how;
    switch (posix_how)
    {
    case SHUT_RD:
        wasi_how = WASI_SOCKETS_0_2_0_RC_2023_10_18_TCP_SHUTDOWN_TYPE_RECEIVE;
        break;
    case SHUT_WR:
        wasi_how = WASI_SOCKETS_0_2_0_RC_2023_10_18_TCP_SHUTDOWN_TYPE_SEND;
        break;
    case SHUT_RDWR:
        wasi_how = WASI_SOCKETS_0_2_0_RC_2023_10_18_TCP_SHUTDOWN_TYPE_BOTH;
        break;
    default:
        errno = EINVAL;
        return -1;
    }

    tcp_socket_state_connected_t connection;
    if (socket->state_tag == TCP_SOCKET_STATE_CONNECTED) {
        connection = socket->state.connected;
    } else {
        errno = ENOTCONN;
        return -1;
    }

    wasi_sockets_0_2_0_rc_2023_10_18_network_error_code_t error;
    reactor_borrow_tcp_socket_t socket_borrow = wasi_sockets_0_2_0_rc_2023_10_18_tcp_borrow_tcp_socket(socket->socket);
    if (!wasi_sockets_0_2_0_rc_2023_10_18_tcp_method_tcp_socket_shutdown(socket_borrow, wasi_how, &error)) {
        errno = __wasi_sockets_utils__map_error(error);
        return -1;
    }

    if (posix_how == SHUT_RD || posix_how == SHUT_RDWR) {
        // TODO: drop input stream (if not already). And update `recv` to take dropped input streams into account.
    }

    if (posix_how == SHUT_WR || posix_how == SHUT_RDWR) {
        // TODO: drop output stream (if not already). And update `send` to take dropped output streams into account.
    }

    return 0;
}

int udp_shutdown(udp_socket_t* socket, int posix_how)
{
    // UDP has nothing to shut down.
    errno = EOPNOTSUPP;
    return -1;
}

int shutdown(int socket, int how) {

    descriptor_table_entry_t* entry;
    if (!descriptor_table_get_ref(socket, &entry)) {
        errno = EBADF;
        return -1;
    }

    switch (entry->tag)
    {
    case DESCRIPTOR_TABLE_ENTRY_TCP_SOCKET:
        return tcp_shutdown(&entry->value.tcp_socket, how);
    case DESCRIPTOR_TABLE_ENTRY_UDP_SOCKET:
        return udp_shutdown(&entry->value.udp_socket, how);
    default:
        errno = EOPNOTSUPP;
        return -1;
    }
}
