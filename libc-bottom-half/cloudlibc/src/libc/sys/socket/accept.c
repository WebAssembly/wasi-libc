// Copyright (c) 2015-2017 Nuxi, https://nuxi.nl/
// SPDX-License-Identifier: BSD-2-Clause

#include <common/errno.h>
#include <sys/socket.h>
#include <assert.h>
#include <wasi/api.h>
#include <errno.h>

int accept(int socket, struct sockaddr *restrict addr, socklen_t *restrict addrlen) {
  int ret = -1;
  __wasi_errno_t error = __wasi_sock_accept(socket, 0, &ret);

  if (error != 0) {
    errno = errno_fixup_socket(socket, error);
    return -1;
  }
  return ret;
}

int accept4(int socket, struct sockaddr *restrict addr, socklen_t *restrict addrlen, int flags) {
    int ret = -1;
    __wasi_errno_t error = __wasi_sock_accept(socket, flags, &ret);

    if (error != 0) {
        errno = errno_fixup_socket(socket, error);
        return -1;
    }
    return ret;
}