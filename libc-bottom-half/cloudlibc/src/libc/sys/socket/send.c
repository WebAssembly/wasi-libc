// Copyright (c) 2015-2017 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <sys/socket.h>

#include <assert.h>
#include <errno.h>
#include <wasi/api.h>

#include <descriptor_table.h>

ssize_t send(int socket, const void* buffer, size_t length, int flags)
{
    // This implementation does not support any flags.
    if (flags != 0) {
        errno = EOPNOTSUPP;
        return -1;
    }

#if 0
    // Prepare input parameters.
    __wasi_ciovec_t iov = {.buf = buffer, .buf_len = length};
    __wasi_ciovec_t *si_data = &iov;
    size_t si_data_len = 1;
    __wasi_siflags_t si_flags = 0;

    // Perform system call.
    size_t so_datalen;
    __wasi_errno_t error = __wasi_sock_send(socket, si_data, si_data_len, si_flags, &so_datalen);
    if (error != 0) {
        errno = error;
        return -1;
     }
    return so_datalen;
#else
    descriptor_table_variant_t variant;
    if (!descriptor_table_get(socket, &variant)) {
        errno = EBADF;
        return -1;
    }

    reactor_own_output_stream_t tx;
    switch (variant.tag) {
    case DESCRIPTOR_TABLE_VARIANT_TCP_CONNECTED:
        tx = variant.value.tcp_connected.tx;
        break;

    default:
        errno = ENOTCONN;
        return -1;
    }

    // TODO: handle non-blocking sends.  Also, when in blocking mode, only block
    // long enough to send at least one byte (i.e. use `check_write`, `write`,
    // and `poll_one` instead of `blocking_write_and_flush`.
    reactor_borrow_output_stream_t tx_borrow = wasi_io_0_2_0_rc_2023_10_18_streams_borrow_output_stream(tx);
    wasi_io_0_2_0_rc_2023_10_18_streams_stream_error_t error;
    reactor_list_u8_t list = { .ptr = (uint8_t*)buffer, .len = length };
    if (!wasi_io_0_2_0_rc_2023_10_18_streams_method_output_stream_blocking_write_and_flush(tx_borrow, &list, &error)) {
        // TODO: map errors appropriately
        errno = EACCES;
        return -1;
    }

    return length;
#endif
}
