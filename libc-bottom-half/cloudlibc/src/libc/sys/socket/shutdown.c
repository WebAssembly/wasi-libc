// Copyright (c) 2015-2016 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <common/errno.h>

#include <sys/socket.h>

#include <assert.h>
#include <wasi.h>
#include <errno.h>

static_assert(SHUT_RD == WASI_SHUT_RD, "Value mismatch");
static_assert(SHUT_WR == WASI_SHUT_WR, "Value mismatch");

int shutdown(int socket, int how) {
  // Validate shutdown flags.
  if (how != SHUT_RD && how != SHUT_WR && how != SHUT_RDWR) {
    errno = EINVAL;
    return -1;
  }

  wasi_errno_t error = wasi_sock_shutdown(socket, how);
  if (error != 0) {
    errno = errno_fixup_socket(socket, error);
    return -1;
  }
  return error;
}
