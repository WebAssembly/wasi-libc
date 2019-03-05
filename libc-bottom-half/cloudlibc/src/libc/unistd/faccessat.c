// Copyright (c) 2016 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <common/errno.h>

#include <wasi/core.h>
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
#ifdef __wasilibc_unmodified_upstream // split out __wasi_lookup_t
  __wasi_lookup_t lookup = {
      .fd = fd,
      .flags = __WASI_LOOKUP_SYMLINK_FOLLOW,
  };
#else
  __wasi_lookupflags_t lookup_flags = __WASI_LOOKUP_SYMLINK_FOLLOW;
#endif
  __wasi_filestat_t file;
  __wasi_errno_t error =
#ifdef __wasilibc_unmodified_upstream // split out __wasi_lookup_t
      __wasi_file_stat_get(lookup, path, strlen(path), &file);
#else
      __wasi_path_filestat_get(fd, lookup_flags, path, strlen(path), &file);
#endif
  if (error != 0) {
    errno = errno_fixup_directory(fd, error);
    return -1;
  }

  // Test whether the requested access rights are present on the
  // directory file descriptor.
  if (amode != 0) {
    __wasi_fdstat_t directory;
#ifdef __wasilibc_unmodified_upstream
    error = __wasi_fd_stat_get(fd, &directory);
#else
    error = __wasi_fd_fdstat_get(fd, &directory);
#endif
    if (error != 0) {
      errno = error;
      return -1;
    }

    __wasi_rights_t min = 0;
    if ((amode & R_OK) != 0)
      min |= file.st_filetype == __WASI_FILETYPE_DIRECTORY
#ifdef __wasilibc_unmodified_upstream
                 ? __WASI_RIGHT_FILE_READDIR
#else
                 ? __WASI_RIGHT_FD_READDIR
#endif
                 : __WASI_RIGHT_FD_READ;
    if ((amode & W_OK) != 0)
      min |= __WASI_RIGHT_FD_WRITE;
    if ((amode & X_OK) != 0 && file.st_filetype != __WASI_FILETYPE_DIRECTORY)
#ifdef __wasilibc_unmodified_upstream // RIGHT_PROC_EXEC
      min |= __WASI_RIGHT_PROC_EXEC;
#else
      (void)0;
#endif

    if ((min & directory.fs_rights_inheriting) != min) {
      errno = EACCES;
      return -1;
    }
  }
  return 0;
}
