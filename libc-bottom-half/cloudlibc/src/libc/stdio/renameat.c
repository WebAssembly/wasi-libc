// Copyright (c) 2015-2016 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <common/errno.h>

#include <wasi/core.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

#ifdef __wasilibc_unmodified_upstream // Rename for AT_FDCWD support
int renameat(int oldfd, const char *old, int newfd, const char *new) {
#else
int __wasilibc_nocwd_renameat(int oldfd, const char *old, int newfd, const char *new) {
#endif
#ifdef __wasilibc_unmodified_upstream
  __wasi_errno_t error = __wasi_file_rename(oldfd, old, strlen(old),
#else
  __wasi_errno_t error = __wasi_path_rename(oldfd, old, strlen(old),
#endif
                                                    newfd, new, strlen(new));
  if (error != 0) {
    errno = errno_fixup_directory(oldfd, errno_fixup_directory(newfd, error));
    return -1;
  }
  return 0;
}
