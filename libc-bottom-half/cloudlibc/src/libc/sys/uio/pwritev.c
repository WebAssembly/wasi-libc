// Copyright (c) 2015-2016 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <sys/types.h>
#include <sys/uio.h>
#ifdef __wasilibc_use_wasip2
#include <wasi/wasip2.h>
#include <wasi/file_utils.h>
#include <common/errors.h>
#else
#include <wasi/api.h>
#endif
#include <errno.h>

ssize_t pwritev(int fildes, const struct iovec *iov, int iovcnt, off_t offset) {
  if (iovcnt < 0 || offset < 0) {
    errno = EINVAL;
    return -1;
  }
  size_t bytes_written;
#ifdef __wasilibc_use_wasip2
  // Note: following the preview1 adapter's behavior, only the first non-empty
  // iov is written.

  // Find the first non-empty iov
  int32_t i = 0;
  while (i < iovcnt) {
    if (iov[i].iov_len != 0)
      break;
    i++;
  }

  // If there is none, return
  if (i >= iovcnt) {
    return 0;
  }

  // Translate the file descriptor to an internal handle
  filesystem_borrow_descriptor_t file_handle;
  if (!fd_to_file_handle(fildes, &file_handle)) {
    errno = EBADF;
    return -1;
  }

  // Create a WASI buffer to copy the contents into
  wasip2_list_u8_t buffer;
  buffer.len = iov[i].iov_len;
  buffer.ptr = malloc(buffer.len);
  memcpy(buffer.ptr, iov[i].iov_base, buffer.len);

  // Write the data
  filesystem_error_code_t error_code;
  filesystem_filesize_t num_bytes;
  bool ok = filesystem_method_descriptor_write(file_handle,
                                               &buffer,
                                               offset,
                                               &num_bytes,
                                               &error_code);
  wasip2_list_u8_free(&buffer);
  if (!ok) {
    translate_error(error_code);
    return -1;
  }
  if (num_bytes > INT32_MAX) {
    // In this case, the number of bytes written can't
    // be represented by an ssize_t
    errno = EINVAL;
    return -1;
  }
  bytes_written = (ssize_t) num_bytes;
#else
  __wasi_errno_t error = __wasi_fd_pwrite(
      fildes, (const __wasi_ciovec_t *)iov, iovcnt, offset, &bytes_written);
  if (error != 0) {
    errno = error;
    return -1;
  }
#endif
  return bytes_written;
}
