// Copyright (c) 2015-2016 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <sys/stat.h>

#include <wasi.h>
#include <errno.h>

#include "stat_impl.h"

int fstat(int fildes, struct stat *buf) {
  wasi_filestat_t internal_stat;
  wasi_errno_t error = wasi_file_stat_fget(fildes, &internal_stat);
  if (error != 0) {
    errno = error;
    return -1;
  }
  to_public_stat(&internal_stat, buf);
  return 0;
}
