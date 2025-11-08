#ifndef __wasi_file_utils_h
#define __wasi_file_utils_h

#ifdef __wasilibc_use_wasip2
#include <wasi/descriptor_table.h>
#include <dirent.h>

// Succeed only if fd is bound to a file handle in the descriptor table
static bool fd_to_file_handle(int fd, filesystem_borrow_descriptor_t* result) {
  descriptor_table_entry_t* entry = 0;
  if (!descriptor_table_get_ref(fd, &entry))
    return false;
  if (entry->tag != DESCRIPTOR_TABLE_ENTRY_FILE_HANDLE)
    return false;
  *result = entry->file.file_handle;
  return true;
}

// Succeed if fd is bound to a file handle or a file input/output stream in the descriptor table
static bool fd_to_file_handle_allow_open(int fd, filesystem_borrow_descriptor_t* result) {
  descriptor_table_entry_t *entry = 0;
  bool ref_exists = descriptor_table_get_ref(fd, &entry);
  filesystem_borrow_descriptor_t file_handle;
  if (!ref_exists)
    return false;
  if (entry->tag == DESCRIPTOR_TABLE_ENTRY_FILE_STREAM)
    *result = entry->stream.file_info.file_handle;
  else if (entry->tag == DESCRIPTOR_TABLE_ENTRY_FILE_HANDLE)
    *result = entry->file.file_handle;
  else if (entry->tag == DESCRIPTOR_TABLE_ENTRY_DIRECTORY_STREAM)
    *result = entry->directory_stream_info.directory_file_handle;
  else
    return false;
  return true;
}

// Succeed only if fd is bound to a directory stream in the descriptor table
static bool fd_to_directory_stream(int fd, filesystem_borrow_directory_entry_stream_t* result_stream,
                                   filesystem_borrow_descriptor_t* result_fd,
                                   read_directory_state_t* result_state) {
  descriptor_table_entry_t *entry = 0;
  if (!descriptor_table_get_ref(fd, &entry))
    return false;
  if (entry->tag != DESCRIPTOR_TABLE_ENTRY_DIRECTORY_STREAM) {
    return false;
  }
  *result_stream = filesystem_borrow_directory_entry_stream(entry->directory_stream_info.directory_stream);
  *result_fd = entry->directory_stream_info.directory_file_handle;
  *result_state = entry->directory_stream_info.directory_state;
  return true;
}

// Does nothing if fd is not in the descriptor table or is not bound to a directory stream
static void directory_stream_enter_state(int fd, read_directory_state_t state) {
  descriptor_table_entry_t *entry = 0;
  if (!descriptor_table_get_ref(fd, &entry))
    return;

  if (entry->tag == DESCRIPTOR_TABLE_ENTRY_DIRECTORY_STREAM)
    entry->directory_stream_info.directory_state = state;
}

// Does nothing if fd is not in the descriptor table or is not bound to a directory stream
static void remove_and_drop_directory_stream(int fd) {
  descriptor_table_entry_t *entry = 0;
  if (!descriptor_table_get_ref(fd, &entry))
    return;

  if (entry->tag == DESCRIPTOR_TABLE_ENTRY_DIRECTORY_STREAM) {
    filesystem_directory_entry_stream_drop_own(entry->directory_stream_info.directory_stream);
    filesystem_borrow_descriptor_t file_handle = entry->directory_stream_info.directory_file_handle;
    entry->tag = DESCRIPTOR_TABLE_ENTRY_FILE_HANDLE;
    entry->file.file_handle = file_handle;
    entry->file.readable = true;
    entry->file.writable = false;
  }
}

// The following three functions are used for lazily initializing stdin/stdout/stderr in
// the descriptor table. These file descriptors never need to be removed from the
// descriptor table.
static bool init_stdin() {
  descriptor_table_entry_t entry;
  descriptor_table_entry_t *entry_p;

  if (!descriptor_table_get_ref(0, &entry_p)) {
      entry.tag = DESCRIPTOR_TABLE_ENTRY_FILE_STREAM;
      entry.stream.read_stream = streams_borrow_input_stream(stdin_get_stdin());
      entry.stream.offset = 0;
      entry.stream.read_pollable.__handle = 0;
      entry.stream.write_pollable.__handle = 0;
      entry.stream.file_info.readable = true;
      entry.stream.file_info.writable = false;
      // entry.stream.file_info.file_handle is uninitialized, but it will never be used

      return descriptor_table_update(0, entry);
  }
  return true;
}

static bool init_stdout() {
  descriptor_table_entry_t entry;
  descriptor_table_entry_t *entry_p;

  if (!descriptor_table_get_ref(1, &entry_p)) {
      entry.tag = DESCRIPTOR_TABLE_ENTRY_FILE_STREAM;
      entry.stream.write_stream = streams_borrow_output_stream(stdout_get_stdout());
      entry.stream.offset = 0;
      entry.stream.read_pollable.__handle = 0;
      entry.stream.write_pollable.__handle = 0;
      entry.stream.file_info.readable = false;
      entry.stream.file_info.writable = true;
      // entry.stream.file_info.file_handle is uninitialized, but it will never be used

      return descriptor_table_update(1, entry);
  }
  return true;
}

static bool init_stderr() {
  descriptor_table_entry_t entry;
  descriptor_table_entry_t *entry_p;

  if (!descriptor_table_get_ref(2, &entry_p)) {
      entry.tag = DESCRIPTOR_TABLE_ENTRY_FILE_STREAM;
      entry.stream.write_stream = streams_borrow_output_stream(stderr_get_stderr());
      entry.stream.offset = 0;
      entry.stream.read_pollable.__handle = 0;
      entry.stream.write_pollable.__handle = 0;
      entry.stream.file_info.readable = false;
      entry.stream.file_info.writable = true;
      // entry.stream.file_info.file_handle is uninitialized, but it will never be used

      return descriptor_table_update(2, entry);
  }
  return true;
}

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
