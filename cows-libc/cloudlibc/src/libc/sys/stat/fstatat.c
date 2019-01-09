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

int fstatat(int fd, const char *restrict path, struct stat *restrict buf,
            int flag) {
  // Create lookup properties.
  cloudabi_lookup_t lookup = {.fd = fd, .flags = 0};
  if ((flag & AT_SYMLINK_NOFOLLOW) == 0)
    lookup.flags |= CLOUDABI_LOOKUP_SYMLINK_FOLLOW;

  // Perform system call.
  cloudabi_filestat_t internal_stat;
  cloudabi_errno_t error =
      cloudabi_sys_file_stat_get(lookup, path, strlen(path), &internal_stat);
  if (error != 0) {
    errno = errno_fixup_directory(fd, error);
    return -1;
  }
  to_public_stat(&internal_stat, buf);
  return 0;
}
