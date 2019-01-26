// Copyright (c) 2015-2017 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <wasi.h>
#include <errno.h>
#include <unistd.h>

int pipe(int *fildes) {
  wasi_fd_t readfd, writefd;
  wasi_errno_t error = wasi_fd_create2(
      WASI_FILETYPE_SOCKET_STREAM, &readfd, &writefd);
  if (error != 0) {
    errno = error;
    return -1;
  }
  fildes[0] = readfd;
  fildes[1] = writefd;
  return 0;
}
