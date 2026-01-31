// Copyright (c) 2015-2016 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <wasi/api.h>
#include <errno.h>
#include <unistd.h>

#ifndef __wasip1__
#include <wasi/descriptor_table.h>
#include <wasi/file_utils.h>
#include <common/errors.h>
#include <string.h>
#endif

#ifdef __wasip3__
#include <wasi/wasip3_block.h>
#endif

ssize_t read(int fildes, void *buf, size_t nbyte) {
#if defined(__wasip1__)
  __wasi_iovec_t iov = {.buf = buf, .buf_len = nbyte};
  size_t bytes_read;
  __wasi_errno_t error = __wasi_fd_read(fildes, &iov, 1, &bytes_read);
  if (error != 0) {
    errno = error == ENOTCAPABLE ? EBADF : error;
    return -1;
  }
  return bytes_read;
#elif defined(__wasip2__)
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
#elif defined(__wasip3__)
  filesystem_tuple2_stream_u8_future_result_void_error_code_t *stream;
  off_t *off;
  if (__wasilibc_read_stream3(fildes, &stream, &off)<0)
    return -1;
  wasip3_waitable_status_t status =
      filesystem_stream_u8_read(stream->f0, buf, nbyte);
  size_t amount = wasip3_waitable_block_on(status, stream->f0);
  if (amount > 0) {
    if (off)
      *off += amount;
    return amount;
  } else {
    filesystem_result_void_error_code_t error;
    status = filesystem_future_result_void_error_code_read(stream->f1, &error);
    amount = wasip3_waitable_block_on(status, stream->f1);
    if (amount > 0 && error.is_err) {
      translate_error(error.val.err);
      return -1;
    }
    // EOF
    return 0;
  }
#else
# error "Unsupported WASI version"
#endif
}
