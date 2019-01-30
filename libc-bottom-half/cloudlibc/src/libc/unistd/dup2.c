// Copyright (c) 2015-2016 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <wasi.h>
#include <errno.h>
#include <unistd.h>

int dup2(int fildes, int fildes2) {
  __wasi_errno_t error = __wasi_fd_replace(fildes, fildes2);
  if (error != 0) {
    errno = error;
    return -1;
  }
  return fildes2;
}
