// Copyright (c) 2015-2016 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <assert.h>
#ifdef __wasilibc_use_wasip2
#include <wasi/wasip2.h>
#include <wasi/descriptor_table.h>
#include <common/errors.h>
#else
#include <wasi/api.h>
#endif
#include <errno.h>
#include <unistd.h>

static_assert(SEEK_CUR == __WASI_WHENCE_CUR, "Value mismatch");
static_assert(SEEK_END == __WASI_WHENCE_END, "Value mismatch");
static_assert(SEEK_SET == __WASI_WHENCE_SET, "Value mismatch");

#ifdef __wasilibc_use_wasip2
static bool get_file_size(filesystem_borrow_descriptor_t handle,
                          filesystem_filesize_t* size) {
  filesystem_descriptor_stat_t stat;
  filesystem_error_code_t error_code;

  if (!filesystem_method_descriptor_stat(handle, &stat, &error_code))
    return false;

  *size = stat.size;
  return true;
}
#endif

off_t __lseek(int fildes, off_t offset, int whence) {

#ifdef __wasilibc_use_wasip2
  off_t offset_to_use = 0;
  // Look up a stream for fildes
  descriptor_table_entry_t *entry;
  descriptor_table_get_ref(fildes, &entry);
  if (entry->tag == DESCRIPTOR_TABLE_ENTRY_FILE_STREAM) {
    // Find the offset relative to the beginning of the file
    // The offset is always *added*: either to 0, the current
    // offset, or the offset of the end of the file.
    switch (whence) {
    case SEEK_SET:
      offset_to_use = offset;
      break;
    case SEEK_CUR:
      offset_to_use = offset + entry->stream.offset;
      break;
    case SEEK_END: {
      // Find the end of the stream (is there a better way to do this?)
      if (entry->stream.file_info.readable) {
        filesystem_filesize_t file_size = 0;
        if (!get_file_size(entry->stream.file_info.file_handle, &file_size)) {
          errno = EINVAL;
          return -1;
        }
        offset_to_use = ((off_t) file_size) + offset;
      } else {
        offset_to_use = entry->stream.offset + offset;
      }
      break;
    }
    default: {
      errno = EINVAL;
      return -1;
    }
    }
    // Drop the existing streams
    if (entry->stream.file_info.readable)
      streams_input_stream_drop_borrow(entry->stream.read_stream);
    if (entry->stream.file_info.writable) {
      poll_pollable_drop_own(entry->stream.pollable);
      streams_output_stream_drop_borrow(entry->stream.write_stream);
    }

    // Open a new stream with the right offset
    if (entry->stream.file_info.readable) {
      filesystem_own_input_stream_t new_stream;
      filesystem_error_code_t error_code;
      bool ok = filesystem_method_descriptor_read_via_stream(entry->stream.file_info.file_handle,
                                                             offset_to_use,
                                                             &new_stream,
                                                             &error_code);
      if (!ok) {
        translate_error(error_code);
        return -1;
      }
      // Update input_stream.stream with the new stream
      entry->stream.read_stream = streams_borrow_input_stream(new_stream);
    }
    if (entry->stream.file_info.writable) {
      filesystem_own_output_stream_t new_stream;
      filesystem_error_code_t error_code;
      bool ok = filesystem_method_descriptor_write_via_stream(entry->stream.file_info.file_handle,
                                                              offset_to_use,
                                                              &new_stream,
                                                              &error_code);
      if (!ok) {
        translate_error(error_code);
        return -1;
      }

      // Update output_stream.stream with the new stream
      entry->stream.write_stream = streams_borrow_output_stream(new_stream);
      entry->stream.pollable = streams_method_output_stream_subscribe(entry->stream.write_stream);
    }

    // Update offset
    entry->stream.offset = offset_to_use;
  } else {
    errno = EINVAL;
    return -1;
  }

  // Return the computed offset
  return offset_to_use;
#else
  __wasi_filesize_t new_offset;
  __wasi_errno_t error =
      __wasi_fd_seek(fildes, offset, whence, &new_offset);
  if (error != 0) {
    errno = error == ENOTCAPABLE ? ESPIPE : error;
    return -1;
  }
  return new_offset;
#endif
}

weak_alias(__lseek, lseek);
