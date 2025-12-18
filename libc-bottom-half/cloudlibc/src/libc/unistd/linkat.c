// Copyright (c) 2015-2016 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <wasi/api.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#ifdef __wasip2__
#include <wasi/file_utils.h>
#include <common/errors.h>
#endif

int __wasilibc_nocwd_linkat(int fd1, const char *path1, int fd2, const char *path2, int flag) {
#ifdef __wasip2__
  // Translate the file descriptors to internal handles
  filesystem_borrow_descriptor_t file_handle1, file_handle2;
  if (fd_to_file_handle(fd1, &file_handle1) < 0)
    return -1;
  if (fd_to_file_handle(fd2, &file_handle2) < 0)
    return -1;

  // Convert the strings into WASI strings
  wasip2_string_t path1_wasi, path2_wasi;
  if (wasip2_string_from_c(path1, &path1_wasi) < 0)
    return -1;
  if (wasip2_string_from_c(path2, &path2_wasi) < 0)
    return -1;

  // Create the link
  filesystem_error_code_t error_code;
  bool ok = filesystem_method_descriptor_link_at(file_handle1,
                                                 0,
                                                 &path1_wasi,
                                                 file_handle2,
                                                 &path2_wasi,
                                                 &error_code);
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
