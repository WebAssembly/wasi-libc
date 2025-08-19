// Copyright (c) 2015-2016 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <sys/stat.h>

#ifdef __wasilibc_use_wasip2
#include <wasi/wasip2.h>
#include <wasi/file_utils.h>
#include <common/errors.h>
#else
#include <wasi/api.h>
#endif
#include <errno.h>
#include <string.h>

int __wasilibc_nocwd_mkdirat_nomode(int fd, const char *path) {

#ifdef __wasilibc_use_wasip2
  // Translate the file descriptor to an internal handle
  filesystem_borrow_descriptor_t file_handle;
  if (!fd_to_file_handle(fd, &file_handle))
    return EBADF;

  // Create the directory
  filesystem_error_code_t error;
  wasip2_string_t path2;
  wasip2_string_dup(&path2, path);
  bool ok = filesystem_method_descriptor_create_directory_at(file_handle,
                                                               &path2,
                                                               &error);
  wasip2_string_free(&path2);
  if (!ok) {
    translate_error(error);
    return -1;
  }
  return 0;
#else
  __wasi_errno_t error = __wasi_path_create_directory(fd, path);

  if (error != 0) {
    errno = error;
    return -1;
  }
  return 0;
#endif
}
