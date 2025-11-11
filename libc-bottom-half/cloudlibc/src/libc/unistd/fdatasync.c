// Copyright (c) 2015-2016 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <assert.h>
#ifdef __wasilibc_use_wasip2
#include <wasi/wasip2.h>
#include <wasi/file_utils.h>
#include <common/errors.h>
#else
#include <wasi/api.h>
#endif
#include <errno.h>
#include <unistd.h>

int fdatasync(int fildes) {
#ifdef __wasilibc_use_wasip2
  // Translate the file descriptor to an internal handle
  filesystem_borrow_descriptor_t file_handle;
  if (!fd_to_file_handle(fildes, &file_handle)) {
    errno = EBADF;
    return -1;
  }

  // Sync the data
  filesystem_error_code_t error_code;
  if (!filesystem_method_descriptor_sync_data(file_handle, &error_code)) {
    translate_error(error_code);
    return -1;
  }
#else
  __wasi_errno_t error = __wasi_fd_datasync(fildes);
  if (error != 0) {
    errno = error == ENOTCAPABLE ? EBADF : error;
    return -1;
  }
#endif
  return 0;
}
