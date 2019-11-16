// Copyright (c) 2015-2017 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <common/errno.h>

#include <sys/socket.h>

#include <assert.h>
#include <wasi/api.h>
#include <errno.h>

ssize_t send(int socket, const void *buffer, size_t length, int flags) {
  // This implementation does not support any flags.
  if (flags != 0) {
    errno = EOPNOTSUPP;
    return -1;
  }

  // Prepare input parameters.
  __wasi_ciovec_t iov = {.buf = buffer, .buf_len = length};
#ifdef __wasilibc_unmodified_upstream // send/recv
  __wasi_send_in_t si = {
      .si_data = &iov,
      .si_data_len = 1,
  };
#else
  __wasi_ciovec_t *si_data = &iov;
  size_t si_data_len = 1;
  __wasi_siflags_t si_flags = 0;
#endif

  // Perform system call.
#ifdef __wasilibc_unmodified_upstream // send/recv
  __wasi_send_out_t so;
  __wasi_errno_t error = __wasi_sock_send(socket, &si, &so);
#else
  size_t so_datalen;
  __wasi_errno_t error = __wasi_sock_send(socket, si_data, si_data_len, si_flags, &so_datalen);
#endif
  if (error != 0) {
    errno = errno_fixup_socket(socket, error);
    return -1;
  }
#ifdef __wasilibc_unmodified_upstream // send/recv
  return so.so_datalen;
#else
  return so_datalen;
#endif
}
