// Copyright (c) 2015 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <dirent.h>

#include "dirent_impl.h"

#if defined(__webp2__)
#include <errno.h>
#include <wasi/report-error.h>
#endif

int dirfd(DIR *dirp) {
#if !defined(__webp2__)
  return dirp->fd;
#else
  WASILIBC_LOG_UNSUPPORTED("dirfd");
  errno = ENOSYS;
  return -1;
#endif
}
