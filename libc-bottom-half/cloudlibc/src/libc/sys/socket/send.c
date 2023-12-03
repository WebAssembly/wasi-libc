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
    // TODO: flags:
    // - MSG_DONTWAIT: we can probably support this relatively easy.
    // - MSG_NOSIGNAL: ignore it. This is always the case. https://github.com/WebAssembly/wasi-sockets/blob/main/Posix-compatibility.md#writing-to-closed-streams-sigpipe-so_nosigpipe-
    // - MSG_OOB: Not supported in WASI
    // - MSG_EOR: Not supported in TCP/UDP in general.
    if (flags != 0) {
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

    reactor_borrow_output_stream_t tx_borrow = wasi_io_0_2_0_rc_2023_10_18_streams_borrow_output_stream(connection.output);
    while (true) {
        wasi_io_0_2_0_rc_2023_10_18_streams_stream_error_t error;
        uint64_t count;
        if (!wasi_io_0_2_0_rc_2023_10_18_streams_method_output_stream_check_write(tx_borrow, &count, &error)) {
            // TODO: wasi-sockets has no way to recover stream errors yet.
            errno = EPIPE;
            return -1;
        }

        if (count) {
            count = count < length ? count : length;
            reactor_list_u8_t list = { .ptr = (uint8_t*)buffer, .len = count };
            if (!wasi_io_0_2_0_rc_2023_10_18_streams_method_output_stream_write(tx_borrow, &list, &error)) {
                // TODO: wasi-sockets has no way to recover stream errors yet.
                errno = EPIPE;
                return -1;
            } else {
                return count;
            }
        } else if (socket->blocking) {
            reactor_borrow_pollable_t pollable_borrow = wasi_io_0_2_0_rc_2023_10_18_poll_borrow_pollable(connection.output_pollable);
            wasi_io_0_2_0_rc_2023_10_18_poll_poll_one(pollable_borrow);
        } else {
            errno = EAGAIN;
            return -1;
        }
    }
}

ssize_t udp_send(udp_socket_t* socket, const void* buffer, size_t length, int flags)
{
    // TODO: Implement flags. Same as tcp_send.

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
