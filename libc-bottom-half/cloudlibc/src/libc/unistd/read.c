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
  descriptor_table_entry_t *entry = descriptor_table_get_ref(fildes);
  if (!entry)
    return -1;
  if (!entry->vtable->get_read_stream) {
    errno = EOPNOTSUPP;
    return -1;
  }
  wasip2_read_t read;
  if (entry->vtable->get_read_stream(entry->data, &read) < 0)
    return -1;
  return __wasilibc_read(&read, buf, nbyte);
#elif defined(__wasip3__)
  filesystem_tuple2_stream_u8_future_result_void_error_code_t *stream;
  off_t *off;
  if (__wasilibc_read_stream3(fildes, &stream, &off)<0)
    return -1;
  wasip3_waitable_status_t status =
      filesystem_stream_u8_read(stream->f0, buf, nbyte);
  size_t amount = wasip3_waitable_block_on(status, stream->f0);
  if (amount > 0 || nbyte == 0) {
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
