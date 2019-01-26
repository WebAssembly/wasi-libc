// Copyright (c) 2015-2016 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <wasi.h>
#include <errno.h>
#include <unistd.h>

ssize_t read(int fildes, void *buf, size_t nbyte) {
  wasi_iovec_t iov = {.buf = buf, .buf_len = nbyte};
  size_t bytes_read;
  wasi_errno_t error = wasi_fd_read(fildes, &iov, 1, &bytes_read);
  if (error != 0) {
    errno = error == ENOTCAPABLE ? EBADF : error;
    return -1;
  }
  return bytes_read;
}
