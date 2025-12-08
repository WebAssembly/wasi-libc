// Copyright (c) 2015 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <dirent.h>

#include "dirent_impl.h"

long telldir(DIR *dirp) {
#if __wasilibc_use_wasip2
  return dirp->offset + dirp->skip;
#else
  return dirp->cookie;
#endif
}
