// Copyright (c) 2015-2016 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#ifdef __wasilibc_use_wasip2
#include <wasi/wasip2.h>
#include <wasi/descriptor_table.h>
#include <wasi/file_utils.h>
#include <common/errors.h>
#include <time.h>
#else
#include <wasi/api.h>
#endif
#include <errno.h>
#include <unistd.h>

ssize_t write(int fildes, const void *buf, size_t nbyte) {
#ifdef __wasilibc_use_wasip2
  streams_borrow_output_stream_t output_stream;
  poll_borrow_pollable_t pollable;
  bool ok = false;
  filesystem_error_code_t error_code;
  descriptor_table_entry_t* entry = 0;

  // Translate the file descriptor to an internal handle
  off_t *off;
  if (__wasilibc_write_stream(fildes, &output_stream, &off, &pollable) < 0)
    return -1;

  // Check readiness for writing
  uint64_t num_bytes_permitted = 0;
  streams_stream_error_t stream_error;
  while (num_bytes_permitted == 0) {
    ok = streams_method_output_stream_check_write(output_stream,
                                                  &num_bytes_permitted,
                                                  &stream_error);
    if (!ok)
      return wasip2_handle_write_error(stream_error);

    if (num_bytes_permitted == 0)
      poll_method_pollable_block(pollable);
  }

  // Convert the buffer to a WASI list of bytes
  wasip2_list_u8_t contents;
  contents.len = num_bytes_permitted < nbyte ? num_bytes_permitted : nbyte;
  contents.ptr = (uint8_t*) buf;

  // Write the bytes to the stream
  ok = streams_method_output_stream_write(output_stream,
                                          &contents,
                                          &stream_error);
  if (!ok)
    return wasip2_handle_write_error(stream_error);

  ok = streams_method_output_stream_blocking_flush(output_stream,
                                                   &stream_error);
  if (!ok)
    return wasip2_handle_write_error(stream_error);


  if (off)
    *off += contents.len;
  return contents.len;
#else
  __wasi_ciovec_t iov = {.buf = buf, .buf_len = nbyte};
  size_t bytes_written;
  __wasi_errno_t error =
      __wasi_fd_write(fildes, &iov, 1, &bytes_written);
  if (error != 0) {
    errno = error == ENOTCAPABLE ? EBADF : error;
    return -1;
  }
  return bytes_written;
#endif
}
