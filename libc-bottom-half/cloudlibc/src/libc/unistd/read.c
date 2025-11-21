// Copyright (c) 2015-2016 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#ifdef __wasilibc_use_wasip2
#include <wasi/wasip2.h>
#include <wasi/descriptor_table.h>
#include <wasi/file_utils.h>
#include <common/errors.h>
#include <string.h>
#else
#include <wasi/api.h>
#endif
#include <errno.h>
#include <unistd.h>

ssize_t read(int fildes, void *buf, size_t nbyte) {
#ifdef __wasilibc_use_wasip2
  bool ok = false;

  // Translate the file descriptor to an internal handle
  streams_borrow_input_stream_t input_stream;
  off_t *off;
  if (__wasilibc_read_stream(fildes, &input_stream, &off, NULL) < 0)
    return -1;

  // Set up a WASI list of bytes to receive the results
  wasip2_list_u8_t contents;

  // Read the bytes
  streams_stream_error_t stream_error;
  ok = streams_method_input_stream_blocking_read(input_stream,
                                                 nbyte,
                                                 &contents,
                                                 &stream_error);
  if (!ok)
    return wasip2_handle_read_error(stream_error);

  // Copy the bytes allocated in the canonical ABI to `buf`
  memcpy(buf, contents.ptr, contents.len);
  wasip2_list_u8_free(&contents);

  // Update the offset
  if (off)
    *off += contents.len;
  return contents.len;

#else
  __wasi_iovec_t iov = {.buf = buf, .buf_len = nbyte};
  size_t bytes_read;
  __wasi_errno_t error = __wasi_fd_read(fildes, &iov, 1, &bytes_read);
  if (error != 0) {
    errno = error == ENOTCAPABLE ? EBADF : error;
    return -1;
  }
  return bytes_read;
#endif
}
