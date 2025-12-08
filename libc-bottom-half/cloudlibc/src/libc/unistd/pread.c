// Copyright (c) 2015-2016 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#ifdef __wasilibc_use_wasip2
#include <wasi/wasip2.h>
#include <wasi/file_utils.h>
#include <string.h>
#include <common/errors.h>
#else
#include <wasi/api.h>
#endif
#include <errno.h>
#include <unistd.h>

ssize_t pread(int fildes, void *buf, size_t nbyte, off_t offset) {
  if (offset < 0) {
    errno = EINVAL;
    return -1;
  }

#ifdef __wasilibc_use_wasip2
  // Translate the file descriptor to an internal handle
  filesystem_borrow_descriptor_t file_handle;
  if (fd_to_file_handle(fildes, &file_handle) < 0)
    return -1;

  // Set up a WASI tuple to receive the results
  wasip2_tuple2_list_u8_bool_t contents;

  // Read the bytes
  filesystem_error_code_t error_code;
  size_t bytes_read;
  bool ok = filesystem_method_descriptor_read(file_handle,
                                              nbyte,
                                              offset,
                                              &contents,
                                              &error_code);
  bytes_read = contents.f0.len;
  // Copy the bytes allocated in the canonical ABI to `buf`
  memcpy(buf, contents.f0.ptr, bytes_read);
  wasip2_list_u8_free(&contents.f0);

  // Check for errors
  if (!ok) {
    translate_error(error_code);
    return -1;
  }

  return bytes_read;
#else
  __wasi_iovec_t iov = {.buf = buf, .buf_len = nbyte};
  size_t bytes_read;
  __wasi_errno_t error =
      __wasi_fd_pread(fildes, &iov, 1, offset, &bytes_read);
  if (error != 0) {
    __wasi_fdstat_t fds;
    if (error == ENOTCAPABLE && __wasi_fd_fdstat_get(fildes, &fds) == 0) {
      // Determine why we got ENOTCAPABLE.
      if ((fds.fs_rights_base & __WASI_RIGHTS_FD_READ) == 0)
        error = EBADF;
      else
        error = ESPIPE;
    }
    errno = error;
    return -1;
  }
  return bytes_read;
#endif
}

