// Copyright (c) 2015-2017 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <errno.h>
#include <sys/socket.h>

#include <assert.h>
#include <wasi/api.h>
#include <errno.h>
#include <stdint.h>

ssize_t recv(int socket, void *restrict buffer, size_t length, int flags) {
  // Prepare input parameters.
  __wasi_iovec_t iov = {.buf = buffer, .buf_len = length};
  __wasi_iovec_t *ri_data = &iov;
  size_t ri_data_len = 1;
  __wasi_riflags_t ri_flags = 0;

  if ((flags & MSG_PEEK) != 0) { ri_flags |= __WASI_RIFLAGS_RECV_PEEK; }
  if ((flags & MSG_WAITALL) != 0) { ri_flags |= __WASI_RIFLAGS_RECV_WAITALL; }
  if ((flags & MSG_TRUNC) != 0) { ri_flags |= __WASI_RIFLAGS_RECV_DATA_TRUNCATED; }
  if ((flags & MSG_DONTWAIT) != 0) { ri_flags |= __WASI_RIFLAGS_RECV_DONT_WAIT; }

  // Perform system call.
  __wasi_size_t ro_datalen;
  __wasi_roflags_t ro_flags;
  __wasi_errno_t error = __wasi_sock_recv(socket,
                                          ri_data, ri_data_len, ri_flags,
                                          &ro_datalen,
                                          &ro_flags);
  if (error != 0) {
    errno = error;
    return -1;
  }
  return ro_datalen;
}
