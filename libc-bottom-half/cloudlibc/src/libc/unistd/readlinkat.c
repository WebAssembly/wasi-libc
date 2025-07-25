// Copyright (c) 2015-2016 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#ifdef __wasilibc_use_wasip2
#include <wasi/wasip2.h>
#include <wasi/file_utils.h>
#include <common/errors.h>
#else
#include <wasi/api.h>
#endif
#include <errno.h>
#include <string.h>
#include <unistd.h>

ssize_t __wasilibc_nocwd_readlinkat(int fd, const char *restrict path, char *restrict buf,
                                    size_t bufsize) {
  size_t bufused;
#ifdef __wasilibc_use_wasip2
  // Translate the file descriptor to an internal handle
  filesystem_borrow_descriptor_t file_handle;
  if (!fd_to_file_handle_allow_open(fd, &file_handle)) {
    errno = EBADF;
    return -1;
  }

  // Create wasi strings for the input and output paths
  wasip2_string_t wasi_path, link_source;
  wasip2_string_dup(&wasi_path, path);

  // Read the link
  filesystem_error_code_t error_code;
  bool ok = filesystem_method_descriptor_readlink_at(file_handle,
                                                     &wasi_path,
                                                     &link_source,
                                                     &error_code);
  wasip2_string_free(&wasi_path);
  if (!ok) {
    wasip2_string_free(&link_source);
    translate_error(error_code);
    return -1;
  }

  // Copy the contents of the output path back into the buffer provided
  bufused = bufsize < link_source.len ? bufsize : link_source.len;
  memcpy(buf, link_source.ptr, bufused);
#else
  // TODO: Remove the cast on `buf` once the witx is updated with char8 support.
  __wasi_errno_t error = __wasi_path_readlink(fd, path,
                                                      (uint8_t*)buf, bufsize, &bufused);
  if (error != 0) {
    errno = error;
    return -1;
  }
#endif
  return bufused;
}
