// Copyright (c) 2015-2016 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <common/errno.h>

#include <sys/stat.h>

#include <cloudabi_syscalls.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>

#include "stat_impl.h"

int utimensat(int fd, const char *path, const struct timespec times[2],
              int flag) {
  // Convert timestamps and extract NOW/OMIT flags.
  cloudabi_filestat_t fs;
  cloudabi_fsflags_t flags;
  if (!utimens_get_timestamps(times, &fs, &flags)) {
    errno = EINVAL;
    return -1;
  }

  // Create lookup properties.
  cloudabi_lookup_t lookup = {.fd = fd, .flags = 0};
  if ((flag & AT_SYMLINK_NOFOLLOW) == 0)
    lookup.flags |= CLOUDABI_LOOKUP_SYMLINK_FOLLOW;

  // Perform system call.
  cloudabi_errno_t error =
      cloudabi_sys_file_stat_put(lookup, path, strlen(path), &fs, flags);
  if (error != 0) {
    errno = errno_fixup_directory(fd, error);
    return -1;
  }
  return 0;
}
