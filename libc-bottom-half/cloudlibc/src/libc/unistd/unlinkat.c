// Copyright (c) 2015-2016 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <common/errno.h>

#include <wasi/core.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

int unlinkat(int fd, const char *path, int flag) {
#ifdef __wasilibc_unmodified_upstream // unlink
  __wasi_ulflags_t ulflags = 0;
  if ((flag & AT_REMOVEDIR) != 0)
    ulflags |= __WASI_UNLINK_REMOVEDIR;
  __wasi_errno_t error =
      __wasi_file_unlink(fd, path, strlen(path), ulflags);
  if (error != 0) {
    errno = errno_fixup_directory(fd, error);
    return -1;
  }
#else
  __wasi_errno_t error;
  size_t path_len = strlen(path);
  if ((flag & AT_REMOVEDIR) != 0) {
    error = __wasi_file_unlink_directory(fd, path, path_len);
    if (error != 0) {
      errno = errno_fixup_directory(fd, error);
      return -1;
    }
  } else {
    error = __wasi_file_unlink_file(fd, path, path_len);
    if (error != 0) {
      errno = error;
      return -1;
    }
  }
#endif
  return 0;
}
