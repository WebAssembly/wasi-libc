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

ssize_t preadv(int fildes, const struct iovec *iov, int iovcnt, off_t offset) {
  if (iovcnt < 0 || offset < 0) {
    errno = EINVAL;
    return -1;
  }
  size_t bytes_read = 0;
#ifdef __wasilibc_use_wasip2
  // Translate the file descriptor to an internal handle
  filesystem_borrow_descriptor_t file_handle;
  if (fd_to_file_handle(fildes, &file_handle) < 0)
    return -1;

  // Create a WASI buffer to receive the contents
  wasip2_tuple2_list_u8_bool_t buffer;
  filesystem_error_code_t error_code;
  bool ok = true;
  bool free_buffer = false;

  // Differently from the behavior of the preview1 component adapter,
  // read into all non-empty iovecs
  for (size_t i = 0; i < iovcnt; i++) {
    if (iov[i].iov_len == 0)
      continue;

    // Read the data
    ok = filesystem_method_descriptor_read(file_handle,
                                           iov[i].iov_len,
                                           offset,
                                           &buffer,
                                           &error_code);
    free_buffer = true;
    if (buffer.f0.len > INT32_MAX) {
      // In this case, the number of bytes read can't
      // be represented by an ssize_t
      errno = EINVAL;
      wasip2_list_u8_free(&buffer.f0);
      return -1;
    }
    bytes_read += buffer.f0.len;
    // Copy the contents of the buffer into the iov
    memcpy(iov[i].iov_base, buffer.f0.ptr, buffer.f0.len);
  }
  if (free_buffer)
    wasip2_list_u8_free(&buffer.f0);

  if (!ok) {
    translate_error(error_code);
    return -1;
  }
#else
  __wasi_errno_t error = __wasi_fd_pread(
      fildes, (const __wasi_iovec_t *)iov, iovcnt, offset, &bytes_read);
  if (error != 0) {
    errno = error;
    return -1;
  }
#endif
  return bytes_read;
}
