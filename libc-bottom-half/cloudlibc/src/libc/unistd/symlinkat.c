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
#include <string.h>
#include <unistd.h>

int __wasilibc_nocwd_symlinkat(const char *path1, int fd, const char *path2) {

#ifdef __wasilibc_use_wasip2
  // Translate the file descriptor to an internal handle
  filesystem_borrow_descriptor_t file_handle;
  if (!fd_to_file_handle_allow_open(fd, &file_handle)) {
    errno = EBADF;
    return -1;
  }

  // Construct WASI string for the paths
  wasip2_string_t path1_wasi;
  wasip2_string_dup(&path1_wasi, path1);
  wasip2_string_t path2_wasi;
  wasip2_string_dup(&path2_wasi, path2);

  // Construct the link
  filesystem_error_code_t error_code;
  // path1 is the path for the existing file; path2 is the path for the new link
  bool ok = filesystem_method_descriptor_symlink_at(file_handle,
                                                    &path1_wasi,
                                                    &path2_wasi,
                                                    &error_code);
  wasip2_string_free(&path1_wasi);
  wasip2_string_free(&path2_wasi);

  // Check for errors
  if (!ok) {
    translate_error(error_code);
    return -1;
  }

#else

  __wasi_errno_t error = __wasi_path_symlink(path1, fd, path2);
  if (error != 0) {
    errno = error;
    return -1;
  }
#endif
  return 0;
}
