// Copyright (c) 2015-2016 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <sys/stat.h>

#include <cloudabi_syscalls.h>
#include <errno.h>

#include "stat_impl.h"

int futimens(int fd, const struct timespec *times) {
  // Convert timestamps and extract NOW/OMIT flags.
  cloudabi_filestat_t fs;
  cloudabi_fsflags_t flags;
  if (!utimens_get_timestamps(times, &fs, &flags)) {
    errno = EINVAL;
    return -1;
  }

  // Perform system call.
  cloudabi_errno_t error = cloudabi_sys_file_stat_fput(fd, &fs, flags);
  if (error != 0) {
    errno = error;
    return -1;
  }
  return 0;
}
