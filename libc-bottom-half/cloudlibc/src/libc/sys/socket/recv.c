// Copyright (c) 2015-2017 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <sys/socket.h>

#include <assert.h>
#include <errno.h>
#include <stdint.h>
#include <wasi/api.h>

#include <descriptor_table.h>
#include "__utils.h"

static_assert(MSG_PEEK == __WASI_RIFLAGS_RECV_PEEK, "Value mismatch");
static_assert(MSG_WAITALL == __WASI_RIFLAGS_RECV_WAITALL, "Value mismatch");

ssize_t tcp_recv(tcp_socket_t* socket, void* restrict buffer, size_t length, int flags)
{
    // TODO: flags:
    // - MSG_WAITALL: we can probably support these relatively easy.
    // - MSG_OOB: could be shimed by always responding that no OOB data is available.
    // - MSG_PEEK: could be shimmed by performing the receive into a local socket-specific buffer. And on subsequent receives first check that buffer.
    // - MSG_TRUNC: return EOPNOTSUPP. Is UDP only.
    // - MSG_EOR, MSG_CMSG_CLOEXEC: return EOPNOTSUPP. Is not supported on TCP or UDP.
    const int supported_flags = MSG_DONTWAIT;
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

    reactor_borrow_input_stream_t rx_borrow = wasi_io_0_2_0_rc_2023_10_18_streams_borrow_input_stream(connection.input);
    while (true) {
        reactor_list_u8_t result;
        wasi_io_0_2_0_rc_2023_10_18_streams_stream_error_t error;
        if (!wasi_io_0_2_0_rc_2023_10_18_streams_method_input_stream_read(rx_borrow, length, &result, &error)) {
            // TODO: wasi-sockets has no way to recover stream errors yet.
            errno = EPIPE;
            return -1;
        }

        if (result.len) {
            memcpy(buffer, result.ptr, result.len);
            reactor_list_u8_free(&result);
            return result.len;
        } else if (should_block) {
            reactor_borrow_pollable_t pollable_borrow = wasi_io_0_2_0_rc_2023_10_18_poll_borrow_pollable(connection.input_pollable);
            wasi_io_0_2_0_rc_2023_10_18_poll_poll_one(pollable_borrow);
        } else {
            errno = EWOULDBLOCK;
            return -1;
        }
    }
}

ssize_t udp_recv(udp_socket_t* socket, void* restrict buffer, size_t length, int flags)
{
    // TODO: Implement flags. Same as tcp_recv except that MSG_TRUNC is valid for UDP.

    errno = EOPNOTSUPP;
    return -1;
}

ssize_t recv(int socket, void* restrict buffer, size_t length, int flags)
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
        return tcp_recv(&entry->value.tcp_socket, buffer, length, flags);
    case DESCRIPTOR_TABLE_ENTRY_UDP_SOCKET:
        return udp_recv(&entry->value.udp_socket, buffer, length, flags);
    default:
        errno = EOPNOTSUPP;
        return -1;
    }
}
