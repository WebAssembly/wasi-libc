// Copyright (c) 2015-2017 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <common/errno.h>

#include <sys/socket.h>

#include <assert.h>
#include <cloudabi_syscalls.h>
#include <errno.h>
#include <stdint.h>

static_assert(MSG_PEEK == CLOUDABI_SOCK_RECV_PEEK, "Value mismatch");
static_assert(MSG_WAITALL == CLOUDABI_SOCK_RECV_WAITALL, "Value mismatch");

ssize_t recv(int socket, void *restrict buffer, size_t length, int flags) {
  // Validate flags.
  if ((flags & ~(MSG_PEEK | MSG_WAITALL)) != 0) {
    errno = EOPNOTSUPP;
    return -1;
  }

  // Prepare input parameters.
  cloudabi_iovec_t iov = {.buf = buffer, .buf_len = length};
  cloudabi_recv_in_t ri = {
      .ri_data = &iov,
      .ri_data_len = 1,
      .ri_flags = flags,
  };

  // Perform system call.
  cloudabi_recv_out_t ro;
  cloudabi_errno_t error = cloudabi_sys_sock_recv(socket, &ri, &ro);
  if (error != 0) {
    errno = errno_fixup_socket(socket, error);
    return -1;
  }
  return ro.ro_datalen;
}
