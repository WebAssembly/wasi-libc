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
#include <unistd.h>

int ftruncate(int fildes, off_t length) {
  if (length < 0) {
    errno = EINVAL;
    return -1;
  }
#ifdef __wasilibc_use_wasip2
  // Translate the file descriptor to an internal file handle
  filesystem_borrow_descriptor_t file_handle;
  if (fd_to_file_handle(fildes, &file_handle) < 0)
    return -1;

  filesystem_error_code_t error_code;
  if (!filesystem_method_descriptor_set_size(file_handle, length, &error_code)) {
    translate_error(error_code);
    return -1;
  }
#else
  __wasi_filesize_t st_size = length;
  __wasi_errno_t error =
      __wasi_fd_filestat_set_size(fildes, st_size);
  if (error != 0) {
    errno = error;
    return -1;
  }
#endif
  return 0;
}
