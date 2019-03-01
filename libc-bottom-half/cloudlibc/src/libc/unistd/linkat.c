// Copyright (c) 2015-2016 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <common/errno.h>

#include <wasi/core.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

int linkat(int fd1, const char *path1, int fd2, const char *path2, int flag) {
  // Create lookup properties.
#ifdef __wasilibc_unmodified_upstream // split out __wasi_lookup_t
  __wasi_lookup_t lookup1 = {.fd = fd1, .flags = 0};
#else
  __wasi_lookupflags_t lookup1_flags = 0;
#endif
  if ((flag & AT_SYMLINK_FOLLOW) != 0)
#ifdef __wasilibc_unmodified_upstream // split out __wasi_lookup_t
    lookup1.flags |= __WASI_LOOKUP_SYMLINK_FOLLOW;
#else
    lookup1_flags |= __WASI_LOOKUP_SYMLINK_FOLLOW;
#endif

  // Perform system call.
#ifdef __wasilibc_unmodified_upstream // split out __wasi_lookup_t
  __wasi_errno_t error = __wasi_file_link(lookup1, path1, strlen(path1),
#else
  __wasi_errno_t error = __wasi_file_link(fd1, lookup1_flags, path1, strlen(path1),
#endif
                                                  fd2, path2, strlen(path2));
  if (error != 0) {
    errno = errno_fixup_directory(fd1, errno_fixup_directory(fd2, error));
    return -1;
  }
  return 0;
}
