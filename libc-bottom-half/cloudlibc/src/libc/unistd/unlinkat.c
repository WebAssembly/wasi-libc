// Copyright (c) 2015-2016 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <common/errno.h>

#include <wasi.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

int unlinkat(int fd, const char *path, int flag) {
  wasi_ulflags_t ulflags = 0;
  if ((flag & AT_REMOVEDIR) != 0)
    ulflags |= WASI_UNLINK_REMOVEDIR;
  wasi_errno_t error =
      wasi_file_unlink(fd, path, strlen(path), ulflags);
  if (error != 0) {
    errno = errno_fixup_directory(fd, error);
    return -1;
  }
  return 0;
}
