#ifndef __wasi_file_utils_h
#define __wasi_file_utils_h

#ifdef __wasilibc_use_wasip2
#include <wasi/descriptor_table.h>

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
                            filesystem_borrow_descriptor_t* result_fd) {
  descriptor_table_entry_t *entry = 0;
  if (!descriptor_table_get_ref(fd, &entry))
    return false;
  if (entry->tag != DESCRIPTOR_TABLE_ENTRY_DIRECTORY_STREAM) {
    return false;
  }
  *result_stream = filesystem_borrow_directory_entry_stream(entry->directory_stream_info.directory_stream);
  *result_fd = entry->directory_stream_info.directory_file_handle;
  return true;
}

// Does nothing if fd is not in the descriptor table or is not bound to a directory stream
static void remove_and_drop_directory_stream(int fd) {
  descriptor_table_entry_t *entry = 0;
  if (!descriptor_table_get_ref(fd, &entry))
    return;

  if (entry->tag == DESCRIPTOR_TABLE_ENTRY_DIRECTORY_STREAM) {
    filesystem_directory_entry_stream_drop_own(entry->directory_stream_info.directory_stream);
    descriptor_table_remove(fd, entry);
  }
}

// The following three functions are used for lazily initializing stdin/stdout/stderr in
// the descriptor table. These file descriptors never need to be removed from the
// descriptor table.
static bool init_stdin() {
  descriptor_table_entry_t entry;

  entry.tag = DESCRIPTOR_TABLE_ENTRY_FILE_STREAM;
  entry.stream.read_stream = streams_borrow_input_stream(stdin_get_stdin());
  entry.stream.offset = 0;
  entry.stream.read_pollable_is_initialized = false;
  entry.stream.write_pollable_is_initialized = false;
  entry.stream.file_info.readable = true;
  entry.stream.file_info.writable = false;
  // entry.stream.file_info.file_handle is uninitialized, but it will never be used

  return descriptor_table_update(0, entry);
}

static bool init_stdout() {
  descriptor_table_entry_t entry;

  entry.tag = DESCRIPTOR_TABLE_ENTRY_FILE_STREAM;
  entry.stream.write_stream = streams_borrow_output_stream(stdout_get_stdout());
  entry.stream.offset = 0;
  entry.stream.read_pollable_is_initialized = false;
  entry.stream.write_pollable_is_initialized = false;
  entry.stream.file_info.readable = false;
  entry.stream.file_info.writable = true;
  // entry.stream.file_info.file_handle is uninitialized, but it will never be used

  return descriptor_table_update(1, entry);
}

static bool init_stderr() {
  descriptor_table_entry_t entry;

  entry.tag = DESCRIPTOR_TABLE_ENTRY_FILE_STREAM;
  entry.stream.write_stream = streams_borrow_output_stream(stderr_get_stderr());
  entry.stream.offset = 0;
  entry.stream.read_pollable_is_initialized = false;
  entry.stream.write_pollable_is_initialized = false;
  entry.stream.file_info.readable = false;
  entry.stream.file_info.writable = true;
  // entry.stream.file_info.file_handle is uninitialized, but it will never be used

  return descriptor_table_update(2, entry);
}

#endif

#endif
