// Copyright (c) 2015 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <dirent.h>
#include <errno.h>
#include <unistd.h>
#include <wasi/report-error.h>

int closedir(DIR *dirp) {
  return close(fdclosedir(dirp));
}
