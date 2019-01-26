// Copyright (c) 2015-2016 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <sys/stat.h>

#include <wasi.h>
#include <errno.h>

#include "stat_impl.h"

int futimens(int fd, const struct timespec *times) {
  // Convert timestamps and extract NOW/OMIT flags.
  wasi_filestat_t fs;
  wasi_fsflags_t flags;
  if (!utimens_get_timestamps(times, &fs, &flags)) {
    errno = EINVAL;
    return -1;
  }

  // Perform system call.
  wasi_errno_t error = wasi_file_stat_fput(fd, &fs, flags);
  if (error != 0) {
    errno = error;
    return -1;
  }
  return 0;
}
