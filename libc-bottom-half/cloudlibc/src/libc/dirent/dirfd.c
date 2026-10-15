// Copyright (c) 2015 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <dirent.h>

#include "dirent_impl.h"

#if defined(__webp2__)
#include <wasi/report-error.h>
#endif

int dirfd(DIR *dirp) {
#if !defined(__webp2__)
  return dirp->fd;
#else
  WEBP2_UNSUPPORTED("dirfd");
#endif
}
