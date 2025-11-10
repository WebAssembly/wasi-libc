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
  filesystem_own_input_stream_t input_stream;
  streams_own_pollable_t pollable;
  bool ok = false;
  bool create_new_stream = false;
  filesystem_error_code_t error_code;
  descriptor_table_entry_t* entry = 0;

  // Translate the file descriptor to an internal handle
  if (!descriptor_table_get_ref(fildes, &entry)) {
      errno = EBADF;
      return -1;
  }
  if (entry->tag == DESCRIPTOR_TABLE_ENTRY_FILE_HANDLE) {
    create_new_stream = true;

    // Get the output stream
    filesystem_own_output_stream_t stream_owned;
    ok = filesystem_method_descriptor_write_via_stream(entry->file.file_handle,
                                                       0,
                                                       &stream_owned,
                                                       &error_code);
    if (!ok) {
      translate_error(error_code);
      return -1;
    }
    output_stream = streams_borrow_output_stream(stream_owned);
    pollable = streams_method_output_stream_subscribe(output_stream);
  } else if (entry->tag == DESCRIPTOR_TABLE_ENTRY_FILE_STREAM) {
    if (!entry->stream.file_info.writable) {
      errno = EBADF;
      return -1;
    }
    output_stream = entry->stream.write_stream;
    if (entry->stream.write_pollable.__handle == 0) {
      pollable = entry->stream.write_pollable = streams_method_output_stream_subscribe(output_stream);
    } else
      pollable = entry->stream.write_pollable;
  } else {
      errno = EBADF;
      return -1;
  }

  // Check readiness for writing
  uint64_t num_bytes_permitted = 0;
  streams_stream_error_t stream_error;
  ok = streams_method_output_stream_check_write(output_stream,
                                                &num_bytes_permitted,
                                                &stream_error);
  if (!ok) {
    errno = EIO;
    return -1;
  }

  if (num_bytes_permitted < nbyte)
    poll_method_pollable_block(poll_borrow_pollable(pollable));

  // Convert the buffer to a WASI list of bytes
  wasip2_list_u8_t contents;
  contents.len = nbyte;
  contents.ptr = malloc(nbyte);
  if (!memcpy(contents.ptr, buf, nbyte)) {
    errno = EINVAL;
    return -1;
  }

  // Write the bytes to the stream
  ok = streams_method_output_stream_write(output_stream,
                                          &contents,
                                          &stream_error);
  wasip2_list_u8_free(&contents);

  if (!ok) {
    errno = EIO;
    return -1;
  }

  // Update the descriptor table with the stream
  if (create_new_stream) {
    descriptor_table_entry_t new_entry;

    // If the file is readable, also need an input stream
    if (entry->file.readable) {
      streams_own_input_stream_t read_stream;
      ok = filesystem_method_descriptor_read_via_stream(entry->file.file_handle,
                                                        0,
                                                        &read_stream,
                                                        &error_code);
      if (!ok) {
        translate_error(error_code);
        return -1;
      }
      new_entry.stream.read_stream = streams_borrow_input_stream(read_stream);
    }
    new_entry.stream.write_pollable = pollable;
    new_entry.stream.read_pollable.__handle = 0;
    new_entry.stream.write_stream = output_stream;
    new_entry.stream.offset = contents.len;
    new_entry.stream.file_info.readable = entry->file.readable;
    new_entry.stream.file_info.writable = entry->file.writable;
    new_entry.stream.file_info.file_handle = entry->file.file_handle;
    new_entry.tag = DESCRIPTOR_TABLE_ENTRY_FILE_STREAM;
    if (!descriptor_table_update(fildes, new_entry)) {
      errno = ENOMEM;
      return -1;
    }
  }

  ok = streams_method_output_stream_blocking_flush(output_stream,
                                                   &stream_error);
  if (!ok) {
    errno = EIO;
    return -1;
  }

  return nbyte;
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
