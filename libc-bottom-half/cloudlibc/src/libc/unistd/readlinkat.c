// Copyright (c) 2015-2016 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <common/errno.h>

#include <wasi/core.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

ssize_t readlinkat(int fd, const char *restrict path, char *restrict buf,
                   size_t bufsize) {
  size_t bufused;
  __wasi_errno_t error = __wasi_file_readlink(fd, path, strlen(path),
                                                      buf, bufsize, &bufused);
  if (error != 0) {
    errno = errno_fixup_directory(fd, error);
    return -1;
  }
  return bufused;
}
