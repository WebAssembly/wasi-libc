// Copyright (c) 2015-2017 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <sys/socket.h>

#include <assert.h>
#include <wasi.h>
#include <errno.h>

static_assert(SOCK_DGRAM == __WASI_FILETYPE_SOCKET_DGRAM, "Value mismatch");
static_assert(SOCK_STREAM == __WASI_FILETYPE_SOCKET_STREAM, "Value mismatch");

int socketpair(int domain, int type, int protocol, int *socket_vector) {
  // We can only allocate UNIX socket pairs. Validate the parameters in
  // userspace.
  if (domain != AF_UNIX) {
    errno = EAFNOSUPPORT;
    return -1;
  }
  if (type != SOCK_DGRAM && type != SOCK_STREAM) {
    errno = EPROTOTYPE;
    return -1;
  }
  if (protocol != 0) {
    errno = EPROTONOSUPPORT;
    return -1;
  }

  // Create socket pair.
  __wasi_fd_t fd1, fd2;
  __wasi_errno_t error = __wasi_fd_create2(type, &fd1, &fd2);
  if (error != 0) {
    errno = error;
    return -1;
  }
  socket_vector[0] = fd1;
  socket_vector[1] = fd2;
  return 0;
}
