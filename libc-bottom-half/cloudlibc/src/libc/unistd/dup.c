// Copyright (c) 2015-2016 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <wasi.h>
#include <errno.h>
#include <unistd.h>

int dup(int fildes) {
  wasi_fd_t newfd;
  wasi_errno_t error = wasi_fd_dup(fildes, &newfd);
  if (error != 0) {
    errno = error;
    return -1;
  }
  return newfd;
}
