// Copyright (c) 2015-2016 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <common/errno.h>

#include <sys/stat.h>

#include <wasi.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>

#include "stat_impl.h"

int utimensat(int fd, const char *path, const struct timespec times[2],
              int flag) {
  // Convert timestamps and extract NOW/OMIT flags.
#ifdef __wasilibc_unmodified_upstream // fstat
  __wasi_filestat_t fs;
  __wasi_fsflags_t flags;
  if (!utimens_get_timestamps(times, &fs, &flags)) {
#else
  __wasi_timestamp_t st_atim;
  __wasi_timestamp_t st_mtim;
  __wasi_fstflags_t flags;
  if (!utimens_get_timestamps(times, &st_atim, &st_mtim, &flags)) {
#endif
    errno = EINVAL;
    return -1;
  }

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
  __wasi_errno_t error =
#ifdef __wasilibc_unmodified_upstream // split out __wasi_lookup_t, fstat
      __wasi_file_stat_put(lookup, path, strlen(path), &fs, flags);
#else
      __wasi_file_stat_set_times(fd, lookup_flags, path, strlen(path), st_atim, st_mtim, flags);
#endif
  if (error != 0) {
    errno = errno_fixup_directory(fd, error);
    return -1;
  }
  return 0;
}
