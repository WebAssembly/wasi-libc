// Copyright (c) 2015-2016 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <common/errno.h>

#include <wasi/core.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#ifdef __wasilibc_unmodified_upstream // Rename for AT_FDCWD support
ssize_t readlinkat(int fd, const char *restrict path, char *restrict buf,
                   size_t bufsize) {
#else
ssize_t __wasilibc_nocwd_readlinkat(int fd, const char *restrict path, char *restrict buf,
                                    size_t bufsize) {
#endif
  size_t bufused;
#ifdef __wasilibc_unmodified_upstream
  __wasi_errno_t error = __wasi_file_readlink(fd, path, strlen(path),
#else
  __wasi_errno_t error = __wasi_path_readlink(fd, path, strlen(path),
#endif
                                                      buf, bufsize, &bufused);
  if (error != 0) {
    errno = errno_fixup_directory(fd, error);
    return -1;
  }
  return bufused;
}
