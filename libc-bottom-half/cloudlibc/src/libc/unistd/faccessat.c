// Copyright (c) 2016 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#ifdef __wasilibc_use_wasip2
#include <wasi/wasip2.h>
#include <wasi/file_utils.h>
#include <common/errors.h>
#else
#include <wasi/api.h>
#endif
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

int __wasilibc_nocwd_faccessat(int fd, const char *path, int amode, int flag) {
  // Validate function parameters.
  if ((amode & ~(F_OK | R_OK | W_OK | X_OK)) != 0 ||
      (flag & ~AT_EACCESS) != 0) {
    errno = EINVAL;
    return -1;
  }

#ifdef __wasilibc_use_wasip2
  // Translate the file descriptor to an internal handle
  // Translate the file descriptor to an internal handle
  filesystem_borrow_descriptor_t file_handle;
  if (!fd_to_file_handle_allow_open(fd, &file_handle)) {
    errno = EBADF;
    return -1;
  }

  // Convert the path to a WASI string
  wasip2_string_t wasi_path;
  wasip2_string_dup(&wasi_path, path);

  // Call stat() to check if the file exists
  filesystem_descriptor_stat_t stat_result;
  filesystem_error_code_t error_code;
  bool ok = filesystem_method_descriptor_stat_at(file_handle,
                                                 FILESYSTEM_PATH_FLAGS_SYMLINK_FOLLOW,
                                                 &wasi_path,
                                                 &stat_result,
                                                 &error_code);
  wasip2_string_free(&wasi_path);
  if (!ok) {
    translate_error(error_code);
    return -1;
  }

  // If amode == F_OK, we can return true since we already checked
  // that the file exists
  if (amode != 0) {
    // Get the permissions on the directory
    filesystem_descriptor_flags_t directory_flags;
    ok = filesystem_method_descriptor_get_flags(file_handle,
                                                &directory_flags,
                                                &error_code);
    if (!ok) {
      translate_error(error_code);
      return -1;
    }

    bool has_rights = true;

    // Readable if the directory is readable; writable if the directory is writable
    if ((amode & R_OK) != 0) {
      if ((directory_flags & FILESYSTEM_DESCRIPTOR_FLAGS_READ) == 0)
        has_rights = false;
    }
    if ((amode & W_OK) != 0) {
      if ((directory_flags & FILESYSTEM_DESCRIPTOR_FLAGS_MUTATE_DIRECTORY) == 0)
        has_rights = false;
    }
    if (!has_rights) {
      errno = EACCES;
      return -1;
    }
  }
#else
  // Check for target file existence and obtain the file type.
  __wasi_lookupflags_t lookup_flags = __WASI_LOOKUPFLAGS_SYMLINK_FOLLOW;
  __wasi_filestat_t file;
  __wasi_errno_t error =
      __wasi_path_filestat_get(fd, lookup_flags, path, &file);
  if (error != 0) {
    errno = error;
    return -1;
  }

  // Test whether the requested access rights are present on the
  // directory file descriptor.
  if (amode != 0) {
    __wasi_fdstat_t directory;
    error = __wasi_fd_fdstat_get(fd, &directory);
    if (error != 0) {
      errno = error;
      return -1;
    }

    __wasi_rights_t min = 0;
    if ((amode & R_OK) != 0)
      min |= file.filetype == __WASI_FILETYPE_DIRECTORY
                 ? __WASI_RIGHTS_FD_READDIR
                 : __WASI_RIGHTS_FD_READ;
    if ((amode & W_OK) != 0)
      min |= __WASI_RIGHTS_FD_WRITE;

    if ((min & directory.fs_rights_inheriting) != min) {
      errno = EACCES;
      return -1;
    }
  }
#endif
  return 0;
}
