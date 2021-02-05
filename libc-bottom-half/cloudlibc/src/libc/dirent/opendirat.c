// Copyright (c) 2015 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <wasi/libc.h>
#include <dirent.h>
#include <fcntl.h>
#include <stddef.h>
#include <unistd.h>

DIR *opendirat(int dir, const char *dirname) {
  // Open directory.
  int fd = __wasilibc_openat_nomode(dir, dirname, O_RDONLY | O_NONBLOCK | O_DIRECTORY);
  if (fd == -1)
    return NULL;

  // Create directory handle.
  DIR *result = fdopendir(fd);
  if (result == NULL)
    close(fd);
  return result;
}
