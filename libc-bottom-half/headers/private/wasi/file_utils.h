#ifndef __wasi_file_utils_h
#define __wasi_file_utils_h

#ifdef __wasilibc_use_wasip2
#include <wasi/descriptor_table.h>
#include <dirent.h>

// Converts the C string `s` into a WASI string stored in `out`.
//
// The returned `wasip2_string_t` should not be deallocated or free'd, and it
// can only be used while `s` is also valid.
//
// Returns 0 if `s` is valid utf-8.
// Returns -1 and sets errno to `ENOENT` if `s` is not valid utf-8.
int wasip2_string_from_c(const char *s, wasip2_string_t* out);

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
  else
    return false;
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
