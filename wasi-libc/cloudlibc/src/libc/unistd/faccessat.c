// Copyright (c) 2016 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <common/errno.h>

#include <cloudabi_syscalls.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

int faccessat(int fd, const char *path, int amode, int flag) {
  // Validate function parameters.
  if ((amode & ~(F_OK | R_OK | W_OK | X_OK)) != 0 ||
      (flag & ~AT_EACCESS) != 0) {
    errno = EINVAL;
    return -1;
  }

  // Check for target file existence and obtain the file type.
  cloudabi_lookup_t lookup = {
      .fd = fd,
      .flags = CLOUDABI_LOOKUP_SYMLINK_FOLLOW,
  };
  cloudabi_filestat_t file;
  cloudabi_errno_t error =
      cloudabi_sys_file_stat_get(lookup, path, strlen(path), &file);
  if (error != 0) {
    errno = errno_fixup_directory(fd, error);
    return -1;
  }

  // Test whether the requested access rights are present on the
  // directory file descriptor.
  if (amode != 0) {
    cloudabi_fdstat_t directory;
    error = cloudabi_sys_fd_stat_get(fd, &directory);
    if (error != 0) {
      errno = error;
      return -1;
    }

    cloudabi_rights_t min = 0;
    if ((amode & R_OK) != 0)
      min |= file.st_filetype == CLOUDABI_FILETYPE_DIRECTORY
                 ? CLOUDABI_RIGHT_FILE_READDIR
                 : CLOUDABI_RIGHT_FD_READ;
    if ((amode & W_OK) != 0)
      min |= CLOUDABI_RIGHT_FD_WRITE;
    if ((amode & X_OK) != 0 && file.st_filetype != CLOUDABI_FILETYPE_DIRECTORY)
      min |= CLOUDABI_RIGHT_PROC_EXEC;

    if ((min & directory.fs_rights_inheriting) != min) {
      errno = EACCES;
      return -1;
    }
  }
  return 0;
}
