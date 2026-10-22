// Copyright (c) 2015 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <dirent.h>
#include <errno.h>
#include <stdlib.h>
#include <wasi/report-error.h>

#include "dirent_impl.h"

int fdclosedir(DIR *dirp) {
  int fd = dirp->fd;
#ifdef __wasip1__
  free(dirp->buffer);
#endif
  dirent_close_streams(dirp);
  free(dirp->dirent);
  free(dirp);
  return fd;
}
