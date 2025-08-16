#ifdef __wasilibc_use_wasip2
#include <wasi/wasip2.h>
#include <wasi/descriptor_table.h>
#include <common/errors.h>
#else
#include <wasi/api.h>
#endif
#include <wasi/libc.h>
#include <errno.h>

int __wasilibc_nocwd___wasilibc_rmdirat(int fd, const char *path) {
#ifdef __wasilibc_use_wasip2
  // Translate the file descriptor to an internal handle
  descriptor_table_entry_t *entry;
  bool ref_exists = descriptor_table_get_ref(fd, &entry);
  filesystem_borrow_descriptor_t file_handle;
  if (!ref_exists) {
    errno = EBADF;
    return EBADF;
  }
  if (entry->tag == DESCRIPTOR_TABLE_ENTRY_DIRECTORY_STREAM)
    file_handle = entry->directory_stream_info.directory_file_handle;
  else if (entry->tag == DESCRIPTOR_TABLE_ENTRY_FILE_HANDLE)
    file_handle = entry->file.file_handle;
  else {
    errno = EINVAL;
    return EINVAL;
  }

  // Create a WASI string for the path
  wasip2_string_t wasi_path;
  wasip2_string_dup(&wasi_path, path);
  filesystem_error_code_t error_code;

  // Remove the directory
  bool ok = filesystem_method_descriptor_remove_directory_at(file_handle, &wasi_path, &error_code);
  wasip2_string_free(&wasi_path);
  if (!ok) {
    translate_error(error_code);
    return -1;
  }
#else
    __wasi_errno_t error = __wasi_path_remove_directory(fd, path);
    if (error != 0) {
        errno = error;
        return -1;
    }
#endif
    return 0;
}
