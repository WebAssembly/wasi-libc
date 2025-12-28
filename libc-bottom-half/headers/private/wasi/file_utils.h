#ifndef __wasi_file_utils_h
#define __wasi_file_utils_h

#include <wasi/version.h>

#ifdef __wasip2__
#include <assert.h>
#include <wasi/wasip2.h>
#include <wasi/descriptor_table.h>
#include <dirent.h>
#include <fcntl.h>
#include <errno.h>

/// Handles a `wasi:io/streams.stream-error` for a `read`-style operation.
///
/// If the error indicates "closed" then 0 is returned to mean EOF. Otherwise
/// the last-operation-failed handle is closed and errno is set to EIO.
static int wasip2_handle_read_error(streams_stream_error_t error) {
  if (error.tag == STREAMS_STREAM_ERROR_CLOSED) {
    return 0;
  }
  assert(error.tag == STREAMS_STREAM_ERROR_LAST_OPERATION_FAILED);
  io_error_error_drop_own(error.val.last_operation_failed);
  errno = EIO;
  return -1;
}

/// Same as `wasip2_handle_read_error` except "closed" now returns EPIPE
/// instead of 0.
static int wasip2_handle_write_error(streams_stream_error_t error) {
  if (error.tag == STREAMS_STREAM_ERROR_CLOSED) {
    errno = EPIPE;
    return -1;
  }
  assert(error.tag == STREAMS_STREAM_ERROR_LAST_OPERATION_FAILED);
  io_error_error_drop_own(error.val.last_operation_failed);
  errno = EIO;
  return -1;
}

// Converts the C string `s` into a WASI string stored in `out`.
//
// The returned `wasip2_string_t` should not be deallocated or free'd, and it
// can only be used while `s` is also valid.
//
// Returns 0 if `s` is valid utf-8.
// Returns -1 and sets errno to `ENOENT` if `s` is not valid utf-8.
int wasip2_string_from_c(const char *s, wasip2_string_t* out);

// Succeed only if fd is bound to a file handle in the descriptor table
static int fd_to_file_handle(int fd, filesystem_borrow_descriptor_t* result) {
  descriptor_table_entry_t* entry = descriptor_table_get_ref(fd);
  if (entry == NULL)
    return -1;
  if (!entry->vtable->get_file) {
    errno = EINVAL;
    return -1;
  }
  return entry->vtable->get_file(entry->data, result);
}

// Gets an `output-stream` borrow from the `fd` provided.
int __wasilibc_write_stream(int fd,
                            streams_borrow_output_stream_t *out,
                            off_t **off,
                            poll_borrow_pollable_t *pollable);

// Gets an `input-stream` borrow from the `fd` provided.
int __wasilibc_read_stream(int fd,
                           streams_borrow_input_stream_t *out,
                           off_t **off,
                           poll_borrow_pollable_t *pollable);

static unsigned dir_entry_type_to_d_type(filesystem_descriptor_type_t ty) {
  switch(ty) {
  case FILESYSTEM_DESCRIPTOR_TYPE_UNKNOWN:
    return DT_UNKNOWN;
  case FILESYSTEM_DESCRIPTOR_TYPE_BLOCK_DEVICE:
    return DT_BLK;
  case FILESYSTEM_DESCRIPTOR_TYPE_CHARACTER_DEVICE:
    return DT_CHR;
  case FILESYSTEM_DESCRIPTOR_TYPE_DIRECTORY:
    return DT_DIR;
  case FILESYSTEM_DESCRIPTOR_TYPE_FIFO:
    return DT_FIFO;
  case FILESYSTEM_DESCRIPTOR_TYPE_SYMBOLIC_LINK:
    return DT_LNK;
  case FILESYSTEM_DESCRIPTOR_TYPE_REGULAR_FILE:
    return DT_REG;
  case FILESYSTEM_DESCRIPTOR_TYPE_SOCKET:
    return DT_SOCK;
  default:
    return DT_UNKNOWN;
  }
}

#endif

#ifdef __wasip3__
int __wasilibc_write_stream3(int fd,
                            filesystem_stream_u8_t *out,
                            off_t **off);
#endif

#endif
