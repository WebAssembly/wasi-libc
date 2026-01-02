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
  off_t *off;
  filesystem_stream_u8_t input_stream;
  if (__wasilibc_read_stream3(fildes, &input_stream, &off) < 0)
    return -1;

  wasip3_waitable_status_t status = filesystem_stream_u8_read(input_stream, buf, nbyte);
  if (status == WASIP3_WAITABLE_STATUS_BLOCKED) {
    // What to wait on?
    wasip3_waitable_set_t set = wasip3_waitable_set_new();
    wasip3_waitable_join(input_stream, set);
    wasip3_event_t event;
    wasip3_waitable_set_wait(set, &event);
    assert(event.event == WASIP3_EVENT_STREAM_READ);
    assert(event.waitable == input_stream);
    wasip3_waitable_set_drop(set);
    ssize_t bytes_read = event.code;
    if (off)
      *off += bytes_read;
    return bytes_read;
  } else if (WASIP3_WAITABLE_STATE(status) == WASIP3_WAITABLE_COMPLETED) {
    ssize_t bytes_read = WASIP3_WAITABLE_COUNT(status);
    if (off)
      *off += bytes_read;
    return bytes_read;
  } else if (WASIP3_WAITABLE_STATE(status) == WASIP3_WAITABLE_DROPPED) {
    return 0;
  } else {
    abort();
  }
#else
# error "Unsupported WASI version"
#endif
}
