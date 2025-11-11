#ifdef __wasilibc_use_wasip2
#include <wasi/wasip2.h>
#include <wasi/descriptor_table.h>
#include <wasi/file_utils.h>
#include <common/errors.h>
#else
#include <wasi/api.h>
#endif
#include <wasi/libc.h>
#include <errno.h>

int __wasilibc_nocwd___wasilibc_unlinkat(int fd, const char *path) {
#ifdef __wasilibc_use_wasip2
  // Translate the file descriptor to an internal handle
  descriptor_table_entry_t* entry = descriptor_table_get_ref(fd);
  if (!entry)
    return -1;
  if (!entry->vtable->get_file) {
    errno = EOPNOTSUPP;
    return -1;
  }

  if (entry->vtable->close_streams)
    entry->vtable->close_streams(entry->data);
  filesystem_borrow_descriptor_t file;
  if (entry->vtable->get_file(entry->data, &file) < 0)
    return -1;

  // Create a Wasm string from the path
  wasip2_string_t wasi_path;
  if (wasip2_string_from_c(path, &wasi_path) < 0)
    return -1;

  // Unlink the file
  filesystem_error_code_t error_code;
  bool ok = filesystem_method_descriptor_unlink_file_at(file, &wasi_path, &error_code);
  if (!ok) {
    translate_error(error_code);
    return -1;
  }

  return 0;
#else
    __wasi_errno_t error = __wasi_path_unlink_file(fd, path);
    if (error != 0) {
        errno = error;
        return -1;
    }
    return 0;
#endif
}
