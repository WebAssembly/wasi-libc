// Copyright (c) 2015 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <dirent.h>
#include <fcntl.h>
#include <stddef.h>
#include <unistd.h>

#ifdef __wasilibc_unmodified_upstream // Rename for AT_FDCWD support
DIR *opendirat(int dir, const char *dirname) {
#else
DIR *__wasilibc_nocwd_opendirat(int dir, const char *dirname) {
#endif
  // Open directory.
  int fd = openat(dir, dirname, O_RDONLY | O_NONBLOCK | O_DIRECTORY);
  if (fd == -1)
    return NULL;

  // Create directory handle.
  DIR *result = fdopendir(fd);
  if (result == NULL)
    close(fd);
  return result;
}
