#ifdef __wasilibc_use_wasip2
#include <wasi/wasip2.h>
#include <wasi/descriptor_table.h>
#include <common/errors.h>
#else
#include <wasi/api.h>
#endif
#include <wasi/libc.h>
#include <errno.h>

int __wasilibc_nocwd___wasilibc_unlinkat(int fd, const char *path) {
#ifdef __wasilibc_use_wasip2
  // Translate the file descriptor to an internal handle
  descriptor_table_entry_t* entry = 0;
  descriptor_table_get_ref(fd, &entry);
  filesystem_borrow_descriptor_t file_handle;

  if (entry->tag == DESCRIPTOR_TABLE_ENTRY_FILE_HANDLE) {
    file_handle = entry->file.file_handle;
  } else if (entry->tag == DESCRIPTOR_TABLE_ENTRY_FILE_STREAM) {
    file_handle = entry->stream.file_info.file_handle;
    if (entry->stream.file_info.readable)
      streams_input_stream_drop_borrow(entry->stream.read_stream);
    if (entry->stream.file_info.writable)
      streams_output_stream_drop_borrow(entry->stream.write_stream);
  } else {
    errno = __WASI_ERRNO_BADF;
    return -1;
  }

  // Create a Wasm string from the path
  wasip2_string_t wasi_path;
  wasip2_string_dup(&wasi_path, path);

  // Unlink the file
  filesystem_error_code_t error_code;
  bool ok = filesystem_method_descriptor_unlink_file_at(entry->file.file_handle,
                                                        &wasi_path,
                                                        &error_code);
  wasip2_string_free(&wasi_path);
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
