// Copyright (c) 2015-2016 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <sys/uio.h>

#include <assert.h>
#include <errno.h>
#include <limits.h>
#include <stddef.h>
#include <unistd.h>
#include <wasi/api.h>
#include <wasi/file_utils.h>

#ifdef __wasip1__
static_assert(offsetof(struct iovec, iov_base) ==
                  offsetof(__wasi_iovec_t, buf),
              "Offset mismatch");
static_assert(sizeof(((struct iovec *)0)->iov_base) ==
                  sizeof(((__wasi_iovec_t *)0)->buf),
              "Size mismatch");
static_assert(offsetof(struct iovec, iov_len) ==
                  offsetof(__wasi_iovec_t, buf_len),
              "Offset mismatch");
static_assert(sizeof(((struct iovec *)0)->iov_len) ==
                  sizeof(((__wasi_iovec_t *)0)->buf_len),
              "Size mismatch");
static_assert(sizeof(struct iovec) == sizeof(__wasi_iovec_t),
              "Size mismatch");
#endif

ssize_t readv(int fildes, const struct iovec *iov, int iovcnt) {
  if (iovcnt < 0 || IOV_MAX < iovcnt) {
    errno = EINVAL;
    return -1;
  }
#if defined(__wasip1__)
  size_t bytes_read;
  __wasi_errno_t error = __wasi_fd_read(
      fildes, (const __wasi_iovec_t *)iov, iovcnt, &bytes_read);
  if (error != 0) {
    errno = error;
    return -1;
  }
  return bytes_read;
#elif defined(__wasip2__) || defined(__wasip3__) || defined(__webp2__)
  if (!__wasilibc_iovecs_ok(iov, iovcnt, NULL)) {
    errno = EINVAL;
    return -1;
  }

  size_t bytes_read = 0;
  for (int i = 0; i < iovcnt; i++) {
    const struct iovec *v = &iov[i];
    ssize_t res = read(fildes, v->iov_base, v->iov_len);
    if (res < 0) {
      // errno already set
      return res;
    }
    bytes_read += res;
    if ((size_t)res < v->iov_len) {
      break;
    }
  }
  return bytes_read;
#else
# error "Unsupported WASI version"
#endif
}
