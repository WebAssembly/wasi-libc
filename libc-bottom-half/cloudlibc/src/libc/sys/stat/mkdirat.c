// Copyright (c) 2015-2016 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <common/errno.h>

#include <sys/stat.h>

#include <wasi.h>
#include <errno.h>
#include <string.h>

#ifdef __wasilibc_unmodified_upstream
int mkdirat(int fd, const char *path, ...) {
#else
int mkdirat(int fd, const char *path, mode_t mode) {
#endif
#ifdef __wasilibc_unmodified_upstream // __wasi_file_mkdir
  __wasi_errno_t error = __wasi_file_create(
      fd, path, strlen(path), __WASI_FILETYPE_DIRECTORY);
#else
  __wasi_errno_t error = __wasi_file_mkdir(
      fd, path, strlen(path));
#endif
  if (error != 0) {
    errno = errno_fixup_directory(fd, error);
    return -1;
  }
  return 0;
}
