#ifndef __wasi_file_utils_h
#define __wasi_file_utils_h

#ifdef __wasilibc_use_wasip2
#include <wasi/descriptor_table.h>
#include <dirent.h>
#include <fcntl.h>
#include <errno.h>

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
  if (entry->tag != DESCRIPTOR_TABLE_ENTRY_FILE) {
    errno = EINVAL;
    return -1;
  }
  *result = filesystem_borrow_descriptor(entry->file.file_handle);
  return 0;
}

// Succeed only if fd is bound to a file handle in the descriptor table
static void descriptor_init_file(descriptor_table_entry_t *out, filesystem_own_descriptor_t fd, int oflag) {
  out->tag = DESCRIPTOR_TABLE_ENTRY_FILE;
  out->file.file_handle = fd;
  out->file.read_stream.__handle = 0;
  out->file.write_stream.__handle = 0;
  out->file.offset = 0;
  out->file.read_pollable.__handle = 0;
  out->file.write_pollable.__handle = 0;
  out->file.readable = ((oflag & O_RDONLY) != 0);
  out->file.writable = ((oflag & O_WRONLY) != 0);
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

// Closes input/output streams and pollables of `file`.
void __wasilibc_file_close_streams(file_t *file);

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

#endif
