// Copyright (c) 2015 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <dirent.h>
#include <stdlib.h>

#include "dirent_impl.h"

#if defined(__webp2__)
#include <errno.h>
#include <wasi/report-error.h>
#endif

int fdclosedir(DIR *dirp) {
#if !defined(__webp2__)
  int fd = dirp->fd;
#ifdef __wasip1__
  free(dirp->buffer);
#endif
  dirent_close_streams(dirp);
  free(dirp->dirent);
  free(dirp);
  return fd;
#else
  WASILIBC_LOG_UNSUPPORTED("fdclosedir");
  errno = ENOSYS;
  return -1;
#endif // !webp2
}
