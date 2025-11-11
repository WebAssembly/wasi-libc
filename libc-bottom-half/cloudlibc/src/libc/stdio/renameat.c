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
#include <stdio.h>
#include <string.h>

int __wasilibc_nocwd_renameat(int oldfd, const char *old, int newfd, const char *new) {
#ifdef __wasilibc_use_wasip2
  // Translate the file descriptors to internal handles
  filesystem_borrow_descriptor_t old_file_handle;
  if (fd_to_file_handle(oldfd, &old_file_handle) < 0)
    return -1;

  filesystem_borrow_descriptor_t new_file_handle;
  if (fd_to_file_handle(newfd, &new_file_handle) < 0)
    return -1;

  // Convert the strings into WASI strings
  wasip2_string_t old_path, new_path;
  if (wasip2_string_from_c(old, &old_path) < 0)
    return -1;
  if (wasip2_string_from_c(new, &new_path) < 0)
    return -1;

  // Rename the file
  filesystem_error_code_t error_code;
  bool ok = filesystem_method_descriptor_rename_at(old_file_handle,
                                                   &old_path,
                                                   new_file_handle,
                                                   &new_path,
                                                   &error_code);
  if (!ok) {
    translate_error(error_code);
    return -1;
  }
#else
  __wasi_errno_t error = __wasi_path_rename(oldfd, old, newfd, new);
  if (error != 0) {
    errno = error;
    return -1;
  }
#endif
  return 0;
}
