// Copyright (c) 2015-2016 Nuxi, https://nuxi.nl/
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

int __wasilibc_nocwd_linkat(int fd1, const char *path1, int fd2, const char *path2, int flag) {
#ifdef __wasilibc_use_wasip2
  // Translate the file descriptors to internal handles
  filesystem_borrow_descriptor_t file_handle1, file_handle2;
  if (!fd_to_file_handle_allow_open(fd1, &file_handle1)) {
    errno = EBADF;
    return -1;
  }
  if (!fd_to_file_handle_allow_open(fd2, &file_handle2)) {
    errno = EBADF;
    return -1;
  }

  // Create WASI strings for the paths
  wasip2_string_t path1_wasi, path2_wasi;
  wasip2_string_dup(&path1_wasi, path1);
  wasip2_string_dup(&path2_wasi, path2);

  // Create the link
  filesystem_error_code_t error_code;
  bool ok = filesystem_method_descriptor_link_at(file_handle1,
                                                 0,
                                                 &path1_wasi,
                                                 file_handle2,
                                                 &path2_wasi,
                                                 &error_code);
  wasip2_string_free(&path1_wasi);
  wasip2_string_free(&path2_wasi);

  if (!ok) {
    translate_error(error_code);
    return -1;
  }
#else
  // Create lookup properties.
  __wasi_lookupflags_t lookup1_flags = 0;
  if ((flag & AT_SYMLINK_FOLLOW) != 0)
    lookup1_flags |= __WASI_LOOKUPFLAGS_SYMLINK_FOLLOW;

  // Perform system call.
  __wasi_errno_t error = __wasi_path_link(fd1, lookup1_flags, path1, fd2, path2);
  if (error != 0) {
    errno = error;
    return -1;
  }
#endif
  return 0;
}
