// Copyright (c) 2015 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <dirent.h>
#include <fcntl.h>
#include <stddef.h>
#include <unistd.h>
#ifdef __wasilibc_unmodified_upstream // Use WASI libc's nocwd functions.
#else
#include <wasi/libc-nocwd.h>
#endif

#ifdef __wasilibc_unmodified_upstream // Rename for AT_FDCWD support
DIR *opendirat(int dir, const char *dirname) {
#else
DIR *__wasilibc_nocwd_opendirat(int dir, const char *dirname) {
#endif
  // Open directory.
#ifdef __wasilibc_unmodified_upstream // Call the nocwd function directly.
  int fd = openat(dir, dirname, O_RDONLY | O_NONBLOCK | O_DIRECTORY);
#else
  int fd = __wasilibc_nocwd_openat(dir, dirname, O_RDONLY | O_NONBLOCK | O_DIRECTORY);
#endif
  if (fd == -1)
    return NULL;

  // Create directory handle.
  DIR *result = fdopendir(fd);
  if (result == NULL)
    close(fd);
  return result;
}
