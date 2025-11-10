// Copyright (c) 2015-2016 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#ifdef __wasilibc_use_wasip2
#include <wasi/wasip2.h>
#include <wasi/descriptor_table.h>
#include <wasi/file_utils.h>
#include <common/errors.h>
#else
#include <wasi/api.h>
#endif
#include <errno.h>
#include <unistd.h>

ssize_t read(int fildes, void *buf, size_t nbyte) {
#ifdef __wasilibc_use_wasip2
  bool ok = false;

  // Check for stdin
  if (fildes == 0) {
      if (!init_stdin()) {
        errno = EINVAL;
        return -1;
      }
  }

  // Translate the file descriptor to an internal handle
  descriptor_table_entry_t* entry = 0;
  if (!descriptor_table_get_ref(fildes, &entry)) {
    errno = EBADF;
    return -1;
  }
  streams_borrow_input_stream_t input_stream;
  if (entry->tag == DESCRIPTOR_TABLE_ENTRY_FILE_HANDLE) {
    // File's input stream hasn't been opened yet

    // Get the input stream
    filesystem_error_code_t error_code;
    streams_own_input_stream_t input_stream_own;

    ok = filesystem_method_descriptor_read_via_stream(entry->file.file_handle,
                                                      0,
                                                      &input_stream_own,
                                                      &error_code);
    if (!ok) {
      translate_error(error_code);
      return -1;
    }
    input_stream = streams_borrow_input_stream(input_stream_own);

    descriptor_table_entry_t new_entry;

    // If the file is writable, also need an output stream
    if (entry->file.writable) {
      streams_own_output_stream_t write_stream;
      ok = filesystem_method_descriptor_write_via_stream(entry->file.file_handle,
                                                         0,
                                                         &write_stream,
                                                         &error_code);
      if (!ok) {
        translate_error(error_code);
        return -1;
      }

      new_entry.stream.write_stream = streams_borrow_output_stream(write_stream);
    }

    // Update the descriptor table with the newly opened stream
    // for this file
    new_entry.tag = DESCRIPTOR_TABLE_ENTRY_FILE_STREAM;
    new_entry.stream.read_stream = input_stream;
    new_entry.stream.read_pollable.__handle = false;
    new_entry.stream.write_pollable.__handle = false;
    new_entry.stream.offset = 0;
    new_entry.stream.file_info.readable = entry->file.readable;
    new_entry.stream.file_info.writable = entry->file.writable;
    new_entry.stream.file_info.file_handle = entry->file.file_handle;
    if (!descriptor_table_update(fildes, new_entry)) {
      errno = ENOMEM;
      return -1;
    }
  } else if (entry->tag == DESCRIPTOR_TABLE_ENTRY_FILE_STREAM) {
      if (!entry->stream.file_info.readable) {
        errno = EBADF;
        return -1;
      }
      input_stream = entry->stream.read_stream;
  } else {
    errno = EBADF;
    return -1;
  }

  // Set up a WASI list of bytes to receive the results
  wasip2_list_u8_t contents;

  // Read the bytes
  streams_stream_error_t stream_error;
  ok = streams_method_input_stream_blocking_read(input_stream,
                                                 nbyte,
                                                 &contents,
                                                 &stream_error);

  if (!ok) {
    if (stream_error.tag == STREAMS_STREAM_ERROR_CLOSED)
      return 0;
    else {
      errno = EIO;
      return -1;
    }
  }

  // Copy the bytes to `buf` so we can free the list
  if (!memcpy(buf, contents.ptr, contents.len)) {
    errno = EINVAL;
    return -1;
  }
  wasip2_list_u8_free(&contents);

  // Update the offset
  if (!descriptor_table_get_ref(fildes, &entry)) {
    errno = EBADF;
    return -1;
  }
  if (entry->tag == DESCRIPTOR_TABLE_ENTRY_FILE_STREAM) {
    entry->stream.offset += contents.len;
  } else {
    abort(); // unreachable
  }
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
