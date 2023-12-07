// Copyright (c) 2015-2017 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <sys/socket.h>

#include <errno.h>
#include <stdint.h>
#include <wasi/api.h>

#include <descriptor_table.h>
#include "__utils.h"

ssize_t tcp_recvfrom(tcp_socket_t* socket, uint8_t* buffer, size_t length, int flags,
    struct sockaddr* addr , socklen_t* addrlen)
{
    // TODO wasi-sockets: flags:
    // - MSG_WAITALL: we can probably support these relatively easy.
    // - MSG_OOB: could be shimmed by always responding that no OOB data is available.
    // - MSG_PEEK: could be shimmed by performing the receive into a local socket-specific buffer. And on subsequent receives first check that buffer.
    // - MSG_TRUNC: return EOPNOTSUPP. Is UDP only.
    // - MSG_EOR, MSG_CMSG_CLOEXEC: return EOPNOTSUPP. Is not supported on TCP or UDP.
    const int supported_flags = MSG_DONTWAIT;
    if ((flags & supported_flags) != flags) {
        errno = EOPNOTSUPP;
        return -1;
    }

    if (addr != NULL || addrlen != NULL) {
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

    streams_borrow_input_stream_t rx_borrow = streams_borrow_input_stream(connection.input);
    while (true) {
        streams_list_u8_t result;
        streams_stream_error_t error;
        if (!streams_method_input_stream_read(rx_borrow, length, &result, &error)) {
            // TODO wasi-sockets: wasi-sockets has no way to recover stream errors yet.
            errno = EPIPE;
            return -1;
        }

        if (result.len) {
            memcpy(buffer, result.ptr, result.len);
            streams_list_u8_free(&result);
            return result.len;
        } else if (should_block) {
            poll_borrow_pollable_t pollable_borrow = poll_borrow_pollable(connection.input_pollable);
            poll_method_pollable_block(pollable_borrow);
        } else {
            errno = EWOULDBLOCK;
            return -1;
        }
    }
}

ssize_t udp_recvfrom(udp_socket_t* socket, uint8_t* buffer, size_t length, int flags,
    struct sockaddr* addr , socklen_t* addrlen)
{
    // TODO wasi-sockets: Implement flags. Same as tcp_recv except that MSG_TRUNC is valid for UDP.

    errno = EOPNOTSUPP;
    return -1;
}

ssize_t recv(int socket, void* restrict buffer, size_t length, int flags) {
    return recvfrom(socket, buffer, length, flags, NULL, NULL);
}

ssize_t recvfrom(int socket, void *__restrict buffer, size_t length, int flags,
    struct sockaddr *__restrict addr , socklen_t *__restrict addrlen)
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
        return tcp_recvfrom(&entry->tcp_socket, buffer, length, flags, addr, addrlen);
    case DESCRIPTOR_TABLE_ENTRY_UDP_SOCKET:
        return udp_recvfrom(&entry->udp_socket, buffer, length, flags, addr, addrlen);
    default:
        errno = EOPNOTSUPP;
        return -1;
    }
}
