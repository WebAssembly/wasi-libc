// Copyright (c) 2015 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <dirent.h>
#include <unistd.h>

#if defined(__webp2__)
#include <errno.h>
#include <wasi/report-error.h>
#endif

int closedir(DIR *dirp) {
#if !defined(__webp2__)
  return close(fdclosedir(dirp));
#else
  WASILIBC_LOG_UNSUPPORTED("closedir");
  errno = ENOSYS;
  return -1;
#endif
}
