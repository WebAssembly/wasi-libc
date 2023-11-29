// Copyright (c) 2015-2016 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <sys/socket.h>

#include <descriptor_table.h>
#include <errno.h>
#include <string.h>
#include <wasi/api.h>

int getsockopt(int socket, int level, int option_name,
    void* restrict option_value, socklen_t* restrict option_len)
{
    // Only support SOL_SOCKET options for now.
    if (level != SOL_SOCKET) {
        errno = ENOPROTOOPT;
        return -1;
    }

    int value;
    switch (option_name) {
    case SO_TYPE: {
        // Return the type of the socket. This information can simply be
        // obtained by looking at the file descriptor type.
        __wasi_fdstat_t fsb;
        if (__wasi_fd_fdstat_get(socket, &fsb) != 0) {
            errno = EBADF;
            return -1;
        }
        if (fsb.fs_filetype != __WASI_FILETYPE_SOCKET_DGRAM && fsb.fs_filetype != __WASI_FILETYPE_SOCKET_STREAM) {
            errno = ENOTSOCK;
            return -1;
        }
        value = fsb.fs_filetype;
        break;
    }
    case SO_ERROR: {
        descriptor_table_variant_t variant;
        if (!descriptor_table_get(socket, &variant)) {
            errno = EBADF;
            return -1;
        }

        if (variant.tag == DESCRIPTOR_TABLE_VARIANT_TCP_ERROR) {
            // TODO: map variant.value.tcp_error.error to appropriate ERRNO
            errno = EBADF;
            descriptor_table_variant_t new_variant = { .tag = DESCRIPTOR_TABLE_VARIANT_TCP_NEW,
                .value = { .tcp_new = variant.value.tcp_error.socket } };
            if (!descriptor_table_update(socket, new_variant)) {
                abort();
            }
            return -1;
        } else {
            return 0;
        }
    }
    default: {
        errno = ENOPROTOOPT;
        return -1;
    }
    }

    // Copy out integer value.
    memcpy(option_value, &value,
        *option_len < sizeof(int) ? *option_len : sizeof(int));
    *option_len = sizeof(int);
    return 0;
}
