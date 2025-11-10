#ifdef __wasilibc_use_wasip2
#include <wasi/wasip2.h>
#include <wasi/file_utils.h>
#include <common/errors.h>
#else
#include <wasi/api.h>
#endif
#include <__errno.h>
#include <__function___isatty.h>

int __isatty(int fd) {
#ifdef __wasilibc_use_wasip2
  // Translate the file descriptor into an internal handle
  filesystem_borrow_descriptor_t file_handle;
  bool ok = fd_to_file_handle_allow_open(fd, &file_handle);
  if (!ok) {
    errno = EBADF;
    return 0;
  }

  // TODO: handle stdio and call get-terminal functions
  if (file_handle.__handle == 0) {
    errno = ENOTTY;
    return 0;
  }

  // Stat the file to determine if it's a tty
  filesystem_descriptor_stat_t statbuf;
  filesystem_error_code_t error_code;
  ok = filesystem_method_descriptor_stat(file_handle, &statbuf, &error_code);
  if (!ok) {
    translate_error(error_code);
    return 0;
  }

  if (statbuf.type != FILESYSTEM_DESCRIPTOR_TYPE_CHARACTER_DEVICE) {
    errno = ENOTTY;
    return 0;
  }

  // To check if seeking/telling is allowed, try to open a stream.
  // If a stream is already open, then seeking/telling is allowed.
  descriptor_table_entry_t *entry = 0;
  descriptor_table_get_ref(fd, &entry);
  if (entry->tag == DESCRIPTOR_TABLE_ENTRY_FILE_STREAM) {
    errno = ENOTTY;
    return 0;
  }

  if (entry->tag == DESCRIPTOR_TABLE_ENTRY_FILE_HANDLE) {
    // Try opening an input stream at a non-zero offset
    filesystem_own_input_stream_t input_stream;
    bool ok = filesystem_method_descriptor_read_via_stream(file_handle,
                                                           1,
                                                           &input_stream,
                                                           &error_code);
    // If this succeeded, the file descriptor is not a tty
    if (ok) {
      streams_input_stream_drop_own(input_stream);
      errno = ENOTTY;
      return 0;
    }

    // Check the error code
    if (error_code == FILESYSTEM_ERROR_CODE_NO_TTY)
      return 1;

    // Otherwise, the file might be write-only, so try opening an output stream
    filesystem_own_output_stream_t output_stream;
    ok = filesystem_method_descriptor_write_via_stream(file_handle,
                                                       1,
                                                       &output_stream,
                                                       &error_code);
    streams_input_stream_drop_own(input_stream);
    streams_output_stream_drop_own(output_stream);

    // If that succeeded, the file descriptor is not a tty
    if (ok) {
      errno = ENOTTY;
      return 0;
    }

    // Check the error code
    if (error_code == FILESYSTEM_ERROR_CODE_NO_TTY)
      return 1;

    // The error code was something else; assume it's not a tty
    translate_error(error_code);
    return 0;
  }

  errno = EBADF;
  return 0;
#else

    __wasi_fdstat_t statbuf;
    int r = __wasi_fd_fdstat_get(fd, &statbuf);
    if (r != 0) {
        errno = r;
        return 0;
    }

    // A tty is a character device that we can't seek or tell on.
    if (statbuf.fs_filetype != __WASI_FILETYPE_CHARACTER_DEVICE ||
        (statbuf.fs_rights_base & (__WASI_RIGHTS_FD_SEEK | __WASI_RIGHTS_FD_TELL)) != 0) {
        errno = __WASI_ERRNO_NOTTY;
        return 0;
    }

    return 1;
#endif
}
#ifdef __wasilibc_use_wasip2
weak_alias(__isatty, isatty);
#else
extern __typeof(__isatty) isatty __attribute__((weak, alias("__isatty")));
#endif

