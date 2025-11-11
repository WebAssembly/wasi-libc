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

ssize_t pwrite(int fildes, const void *buf, size_t nbyte, off_t offset) {
  if (offset < 0) {
    errno = EINVAL;
    return -1;
  }

#ifdef __wasilibc_use_wasip2
  // Translate the file descriptor to an internal handle
  filesystem_borrow_descriptor_t file_handle;
  if (!fd_to_file_handle(fildes, &file_handle)) {
    errno = EBADF;
    return -1;
  }

  // Convert `buf` to a WASI byte list
  wasip2_list_u8_t contents;
  contents.len = nbyte;
  contents.ptr = malloc(nbyte);
  if (!memcpy(contents.ptr, buf, nbyte)) {
    errno = EINVAL;
    return -1;
  }

  // Write the bytes
  filesystem_filesize_t bytes_written;
  filesystem_error_code_t error_code;
  bool ok = filesystem_method_descriptor_write(file_handle,
                                               &contents,
                                               offset,
                                               &bytes_written,
                                               &error_code);
  // Free the byte list
  wasip2_list_u8_free(&contents);

  // Check for errors
  if (!ok) {
    translate_error(error_code);
    return -1;
  }

  return bytes_written;
#else
  __wasi_ciovec_t iov = {.buf = buf, .buf_len = nbyte};
  size_t bytes_written;
  __wasi_errno_t error =
      __wasi_fd_pwrite(fildes, &iov, 1, offset, &bytes_written);
  if (error != 0) {
    __wasi_fdstat_t fds;
    if (error == ENOTCAPABLE && __wasi_fd_fdstat_get(fildes, &fds) == 0) {
      // Determine why we got ENOTCAPABLE.
      if ((fds.fs_rights_base & __WASI_RIGHTS_FD_WRITE) == 0)
        error = EBADF;
      else
        error = ESPIPE;
    }
    errno = error;
    return -1;
  }
  return bytes_written;
#endif
}
