// Copyright (c) 2015 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <dirent.h>
#include <unistd.h>

#if defined(__webp2__)
#include <wasi/report-error.h>
#endif

int closedir(DIR *dirp) {
#if !defined(__webp2__)
  return close(fdclosedir(dirp));
#else
  WEBP2_UNSUPPORTED("closedir");
#endif
}
