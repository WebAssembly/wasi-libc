// Copyright (c) 2015-2016 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <common/errno.h>

#include <sys/stat.h>

#include <wasi/core.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>

#include "stat_impl.h"

#ifdef __wasilibc_unmodified_upstream // Rename for AT_FDCWD support
int fstatat(int fd, const char *restrict path, struct stat *restrict buf,
            int flag) {
#else
int __wasilibc_nocwd_fstatat(int fd, const char *restrict path, struct stat *restrict buf,
                             int flag) {
#endif
  // Create lookup properties.
#ifdef __wasilibc_unmodified_upstream // split out __wasi_lookup_t
  __wasi_lookup_t lookup = {.fd = fd, .flags = 0};
#else
  __wasi_lookupflags_t lookup_flags = 0;
#endif
  if ((flag & AT_SYMLINK_NOFOLLOW) == 0)
#ifdef __wasilibc_unmodified_upstream // split out __wasi_lookup_t
    lookup.flags |= __WASI_LOOKUP_SYMLINK_FOLLOW;
#else
    lookup_flags |= __WASI_LOOKUP_SYMLINK_FOLLOW;
#endif

  // Perform system call.
  __wasi_filestat_t internal_stat;
  __wasi_errno_t error =
#ifdef __wasilibc_unmodified_upstream // split out __wasi_lookup_t
      __wasi_file_stat_get(lookup, path, strlen(path), &internal_stat);
#else
      __wasi_path_filestat_get(fd, lookup_flags, path, strlen(path), &internal_stat);
#endif
  if (error != 0) {
    errno = errno_fixup_directory(fd, error);
    return -1;
  }
  to_public_stat(&internal_stat, buf);
  return 0;
}
