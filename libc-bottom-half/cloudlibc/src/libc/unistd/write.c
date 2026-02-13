// Copyright (c) 2015-2016 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <errno.h>
#include <unistd.h>
#include <wasi/api.h>

#ifndef __wasip1__
#include <wasi/descriptor_table.h>
#include <wasi/file_utils.h>
#include <common/errors.h>
#include <time.h>
#endif

#ifdef __wasip3__
#include <wasi/wasip3_block.h>
#endif

ssize_t write(int fildes, const void *buf, size_t nbyte) {
#if defined(__wasip1__)
  __wasi_ciovec_t iov = {.buf = buf, .buf_len = nbyte};
  size_t bytes_written;
  __wasi_errno_t error =
      __wasi_fd_write(fildes, &iov, 1, &bytes_written);
  if (error != 0) {
    errno = error == ENOTCAPABLE ? EBADF : error;
    return -1;
  }
  return bytes_written;
#elif defined(__wasip2__)
  descriptor_table_entry_t *entry = descriptor_table_get_ref(fildes);
  if (!entry)
    return -1;
  if (!entry->vtable->get_write_stream) {
    errno = EOPNOTSUPP;
    return -1;
  }
  wasip2_write_t write;
  if (entry->vtable->get_write_stream(entry->data, &write) < 0)
    return -1;
  return __wasilibc_write(&write, buf, nbyte);

#elif defined(__wasip3__)
  wasip3_write_t *write_end;
  off_t *off;
  if (__wasilibc_write_stream3(fildes, &write_end, &off) < 0)
    return -1;
  if (WASIP3_SUBTASK_STATE(write_end->subtask) == WASIP3_SUBTASK_STARTING ||
      WASIP3_SUBTASK_STATE(write_end->subtask) == WASIP3_SUBTASK_STARTED) {
    // the stream is still active
    wasip3_waitable_status_t status =
        filesystem_stream_u8_write(write_end->output, buf, nbyte);
    size_t amount = wasip3_waitable_block_on(status, write_end->output);
    if (amount > 0 || nbyte == 0) {
      if (off)
        *off += amount;
      return amount;
    }
    // error or eof
    wasip3_subtask_block_on(write_end->subtask);
    write_end->subtask = WASIP3_SUBTASK_RETURNED;
  }
  if (write_end->pending_result.is_err) {
    translate_error(write_end->pending_result.val.err);
    return -1;
  }
  // EOF
  return 0;
#else
# error "Unknown WASI version"
#endif
}
