// Copyright (c) 2015-2016 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <wasi/api.h>
#include <errno.h>
#include <unistd.h>

#ifndef __wasip1__
#include <wasi/file_utils.h>
#include <common/errors.h>
#endif

#ifdef __wasip3__
#include <wasi/wasip3_block.h>
#endif

ssize_t pwrite(int fildes, const void *buf, size_t nbyte, off_t offset) {
  if (offset < 0) {
    errno = EINVAL;
    return -1;
  }

#if defined(__wasip1__)
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
#elif defined(__wasip2__)
  // Translate the file descriptor to an internal handle
  filesystem_borrow_descriptor_t file_handle;
  if (fd_to_file_handle(fildes, &file_handle) < 0)
    return -1;

  // Convert `buf` to a WASI byte list
  wasip2_list_u8_t contents;
  contents.len = nbyte;
  contents.ptr = (uint8_t*) buf;

  // Write the bytes
  filesystem_filesize_t bytes_written;
  filesystem_error_code_t error_code;
  bool ok = filesystem_method_descriptor_write(file_handle,
                                               &contents,
                                               offset,
                                               &bytes_written,
                                               &error_code);
  // Check for errors
  if (!ok) {
    translate_error(error_code);
    return -1;
  }

  return bytes_written;
#elif defined(__wasip3__)
  filesystem_borrow_descriptor_t file_handle;
  if (fd_to_file_handle(fildes, &file_handle) < 0)
    return -1;

  // Create a read/write stream, use `write-via-stream` to start writing,
  // then perform the write to see how much was accepted.
  filesystem_stream_u8_writer_t writer;
  filesystem_stream_u8_t reader = filesystem_stream_u8_new(&writer);
  filesystem_result_void_error_code_t result;
  wasip3_subtask_status_t subtask_status =
    filesystem_method_descriptor_write_via_stream(file_handle, reader, offset, &result);
  bool closed;
  size_t ret = wasip3_stream_block_on(
    filesystem_stream_u8_write(writer, buf, nbyte),
    writer,
    &closed);
  filesystem_stream_u8_drop_writable(writer);

  // Wait for the subtask to resolve now that the writer half is closed and if
  // we failed to write bytes (0 bytes written) and the result is an error we
  // can return -1.
  if (WASIP3_SUBTASK_STATE(subtask_status) != WASIP3_SUBTASK_RETURNED)
    wasip3_subtask_block_on_and_drop(WASIP3_SUBTASK_HANDLE(subtask_status));
  if (ret == 0 && result.is_err) {
    translate_error(result.val.err);
    return -1;
  }
  return ret;
#else
# error "Unsupported WASI version"
#endif
}
