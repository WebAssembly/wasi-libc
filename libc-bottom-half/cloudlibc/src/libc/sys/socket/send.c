// Copyright (c) 2015-2017 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <common/errno.h>

#include <sys/socket.h>

#include <assert.h>
#include <wasi.h>
#include <errno.h>

ssize_t send(int socket, const void *buffer, size_t length, int flags) {
  // This implementation does not support any flags.
  if (flags != 0) {
    errno = EOPNOTSUPP;
    return -1;
  }

  // Prepare input parameters.
  wasi_ciovec_t iov = {.buf = buffer, .buf_len = length};
  wasi_send_in_t si = {
      .si_data = &iov,
      .si_data_len = 1,
  };

  // Perform system call.
  wasi_send_out_t so;
  wasi_errno_t error = wasi_sock_send(socket, &si, &so);
  if (error != 0) {
    errno = errno_fixup_socket(socket, error);
    return -1;
  }
  return so.so_datalen;
}
