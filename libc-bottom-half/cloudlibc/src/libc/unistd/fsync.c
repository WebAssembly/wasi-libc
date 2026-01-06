// Copyright (c) 2015-2016 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <wasi/api.h>
#include <errno.h>
#include <unistd.h>

#ifndef __wasip1__
#include <wasi/file_utils.h>
#include <common/errors.h>
#endif

int fsync(int fildes) {
#if defined(__wasip1__)
  __wasi_errno_t error = __wasi_fd_sync(fildes);
  if (error != 0) {
    errno = error == ENOTCAPABLE ? EINVAL : error;
    return -1;
  }
#elif defined(__wasip2__)
  // Translate the file descriptor to an internal handle
  filesystem_borrow_descriptor_t file_handle;
  if (fd_to_file_handle(fildes, &file_handle) < 0)
    return -1;

  // Sync the file
  filesystem_error_code_t error_code;
  if (!filesystem_method_descriptor_sync(file_handle, &error_code)) {
    translate_error(error_code);
    return -1;
  }
#else
# error "Unsupported WASI version"
#endif
  return 0;
}
