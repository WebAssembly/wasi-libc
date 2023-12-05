// Copyright (c) 2015-2017 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <sys/socket.h>

#include <assert.h>
#include <errno.h>
#include <wasi/api.h>

#include <descriptor_table.h>
#include "__utils.h"

ssize_t tcp_send(tcp_socket_t* socket, const void* buffer, size_t length, int flags)
{
    const int supported_flags = MSG_DONTWAIT | MSG_NOSIGNAL;
    if ((flags & supported_flags) != flags) {
        errno = EOPNOTSUPP;
        return -1;
    }

    tcp_socket_state_connected_t connection;
    if (socket->state_tag == TCP_SOCKET_STATE_CONNECTED) {
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

ssize_t udp_send(udp_socket_t* socket, const void* buffer, size_t length, int flags)
{
    // TODO wasi-sockets: Implement flags. Same as tcp_send.

    errno = EOPNOTSUPP;
    return -1;
}

ssize_t send(int socket, const void* buffer, size_t length, int flags)
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
        return tcp_send(&entry->value.tcp_socket, buffer, length, flags);
    case DESCRIPTOR_TABLE_ENTRY_UDP_SOCKET:
        return udp_send(&entry->value.udp_socket, buffer, length, flags);
    default:
        errno = EOPNOTSUPP;
        return -1;
    }
}
