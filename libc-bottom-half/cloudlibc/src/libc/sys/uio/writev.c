// Copyright (c) 2015-2016 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <sys/uio.h>

#include <assert.h>
#ifdef __wasilibc_use_wasip2
#include <wasi/wasip2.h>
#include <wasi/descriptor_table.h>
#include <common/errors.h>
#else
#include <wasi/api.h>
#endif
#include <errno.h>
#include <stddef.h>
#include <unistd.h>

static_assert(offsetof(struct iovec, iov_base) ==
                  offsetof(__wasi_ciovec_t, buf),
              "Offset mismatch");
static_assert(sizeof(((struct iovec *)0)->iov_base) ==
                  sizeof(((__wasi_ciovec_t *)0)->buf),
              "Size mismatch");
static_assert(offsetof(struct iovec, iov_len) ==
                  offsetof(__wasi_ciovec_t, buf_len),
              "Offset mismatch");
static_assert(sizeof(((struct iovec *)0)->iov_len) ==
                  sizeof(((__wasi_ciovec_t *)0)->buf_len),
              "Size mismatch");
static_assert(sizeof(struct iovec) == sizeof(__wasi_ciovec_t),
              "Size mismatch");

ssize_t writev(int fildes, const struct iovec *iov, int iovcnt) {
  if (iovcnt < 0) {
    errno = EINVAL;
    return -1;
  }
  size_t bytes_written = 0;

#ifdef __wasilibc_use_wasip2
  // Differently from the behavior of the preview1 component adapter,
  // write all non-empty iovecs

  for (size_t i = 0; i < iovcnt; i++) {
    if (iov[i].iov_len == 0)
      continue;
    bytes_written += write(fildes, iov[i].iov_base, iov[i].iov_len);
  }
#else
  __wasi_errno_t error = __wasi_fd_write(
      fildes, (const __wasi_ciovec_t *)iov, iovcnt, &bytes_written);
  if (error != 0) {
    errno = error;
    return -1;
  }
#endif
  return bytes_written;
}
