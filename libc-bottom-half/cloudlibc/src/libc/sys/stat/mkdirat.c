// Copyright (c) 2015-2016 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <common/errno.h>

#include <sys/stat.h>

#include <cloudabi_syscalls.h>
#include <errno.h>
#include <string.h>

#ifdef __wasilibc_unmodified_upstream__
int mkdirat(int fd, const char *path, ...) {
#else
int mkdirat(int fd, const char *path, mode_t mode) {
#endif
  cloudabi_errno_t error = cloudabi_sys_file_create(
      fd, path, strlen(path), CLOUDABI_FILETYPE_DIRECTORY);
  if (error != 0) {
    errno = errno_fixup_directory(fd, error);
    return -1;
  }
  return 0;
}
