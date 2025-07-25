// Copyright (c) 2015-2016 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#ifndef __wasilibc_use_wasip2
#include <wasi/api.h>
#endif
#include <errno.h>
#include <fcntl.h>

int posix_fallocate(int fd, off_t offset, off_t len) {
// Note: this operation isn't supported in wasip2
#ifdef __wasilibc_use_wasip2
  return ENOTSUP;
#else
  if (offset < 0 || len < 0)
    return EINVAL;
  return __wasi_fd_allocate(fd, offset, len);
#endif
}
