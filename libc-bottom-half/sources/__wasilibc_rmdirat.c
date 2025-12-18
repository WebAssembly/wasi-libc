#include <errno.h>
#include <wasi/api.h>
#include <wasi/libc.h>

#ifdef __wasip2__
#include <common/errors.h>
#include <wasi/descriptor_table.h>
#include <wasi/file_utils.h>
#endif

int __wasilibc_nocwd___wasilibc_rmdirat(int fd, const char *path) {
#ifdef __wasip2__
  // Translate the file descriptor to an internal handle
  filesystem_borrow_descriptor_t file_handle;
  if (fd_to_file_handle(fd, &file_handle) < 0)
    return -1;

  // Create a WASI string for the path
  wasip2_string_t wasi_path;
  if (wasip2_string_from_c(path, &wasi_path) < 0)
    return -1;
  filesystem_error_code_t error_code;

  // Remove the directory
  bool ok = filesystem_method_descriptor_remove_directory_at(
      file_handle, &wasi_path, &error_code);
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
