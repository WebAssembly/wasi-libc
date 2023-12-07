// Copyright (c) 2015-2017 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <sys/socket.h>

#include <assert.h>
#include <errno.h>
#include <wasi/api.h>

#include <descriptor_table.h>
#include "__utils.h"

ssize_t tcp_sendto(tcp_socket_t* socket, const uint8_t* buffer, size_t length, int flags,
    const struct sockaddr* addr, socklen_t addrlen)
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

    streams_borrow_output_stream_t tx_borrow = streams_borrow_output_stream(connection.output);
    while (true) {
        streams_stream_error_t error;
        uint64_t count;
        if (!streams_method_output_stream_check_write(tx_borrow, &count, &error)) {
            // TODO wasi-sockets: wasi-sockets has no way to recover stream errors yet.
            errno = EPIPE;
            return -1;
        }

        if (count) {
            count = count < length ? count : length;
            streams_list_u8_t list = { .ptr = (uint8_t*)buffer, .len = count };
            if (!streams_method_output_stream_write(tx_borrow, &list, &error)) {
                // TODO wasi-sockets: wasi-sockets has no way to recover stream errors yet.
                errno = EPIPE;
                return -1;
            } else {
                return count;
            }
        } else if (should_block) {
            poll_borrow_pollable_t pollable_borrow = poll_borrow_pollable(connection.output_pollable);
            poll_method_pollable_block(pollable_borrow);
        } else {
            errno = EWOULDBLOCK;
            return -1;
        }
    }
}

ssize_t udp_sendto(udp_socket_t* socket, const uint8_t* buffer, size_t length, int flags,
    const struct sockaddr* addr, socklen_t addrlen)
{
    // TODO wasi-sockets: Implement flags. Same as tcp_send.

    errno = EOPNOTSUPP;
    return -1;
}

ssize_t send(int socket, const void* buffer, size_t length, int flags) {
    return sendto(socket, buffer, length, flags, NULL, 0);
}

ssize_t sendto(int socket, const void* buffer, size_t length, int flags,
    const struct sockaddr* addr, socklen_t addrlen)
{
    descriptor_table_entry_t* entry;
    if (!descriptor_table_get_ref(socket, &entry)) {
        errno = EBADF;
        return -1;
    }

    if (buffer == NULL) {
        errno = EINVAL;
        return -1;
    }

    switch (entry->tag)
    {
    case DESCRIPTOR_TABLE_ENTRY_TCP_SOCKET:
        return tcp_sendto(&entry->tcp_socket, buffer, length, flags, addr, addrlen);
    case DESCRIPTOR_TABLE_ENTRY_UDP_SOCKET:
        return udp_sendto(&entry->udp_socket, buffer, length, flags, addr, addrlen);
    default:
        errno = EOPNOTSUPP;
        return -1;
    }
}
