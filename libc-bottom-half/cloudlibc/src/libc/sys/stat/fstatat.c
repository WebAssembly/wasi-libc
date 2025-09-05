// Copyright (c) 2015-2016 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <sys/stat.h>

#ifdef __wasilibc_use_wasip2
#include <wasi/wasip2.h>
#include <wasi/file_utils.h>
#include <common/errors.h>
#else
#include <wasi/api.h>
#endif
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>

#include "stat_impl.h"

int __wasilibc_nocwd_fstatat(int fd, const char *restrict path, struct stat *restrict buf,
                             int flag) {
#ifdef __wasilibc_use_wasip2
  // Translate the file descriptor to an internal handle
  filesystem_borrow_descriptor_t file_handle;
  if (!fd_to_file_handle_allow_open(fd, &file_handle)) {
    errno = EBADF;
    return -1;
  }

  // Copy the string into a Wasm string
  wasip2_string_t path_wasm_string;
  wasip2_string_dup(&path_wasm_string, path);

  // Get the metadata hash for this file
  filesystem_metadata_hash_value_t metadata;
  filesystem_error_code_t error;
  filesystem_path_flags_t path_flags = (flag | AT_SYMLINK_NOFOLLOW) ? 0
    : FILESYSTEM_PATH_FLAGS_SYMLINK_FOLLOW;
  if (!filesystem_method_descriptor_metadata_hash_at(file_handle,
                                                     path_flags,
                                                     &path_wasm_string,
                                                     &metadata,
                                                     &error)) {
    translate_error(error);
    return -1;
  }

  // Create lookup properties.
  __wasi_lookupflags_t lookup_flags = 0;
  if ((flag & AT_SYMLINK_NOFOLLOW) == 0)
    lookup_flags |= __WASI_LOOKUPFLAGS_SYMLINK_FOLLOW;

  // Perform system call.
  filesystem_descriptor_stat_t internal_stat;
  bool stat_result = filesystem_method_descriptor_stat_at(file_handle,
                                                          lookup_flags,
                                                          &path_wasm_string,
                                                          &internal_stat,
                                                          &error);
  wasip2_string_free(&path_wasm_string);
  if (!stat_result) {
    translate_error(error);
    return -1;
  }

  // Convert the internal data to an external struct
  to_public_stat(&metadata, &internal_stat, buf);
  return 0;
#else
  __wasi_lookupflags_t lookup_flags = 0;
  if ((flag & AT_SYMLINK_NOFOLLOW) == 0)
      lookup_flags |= __WASI_LOOKUPFLAGS_SYMLINK_FOLLOW;

  // Perform system call.
  __wasi_filestat_t internal_stat;
  __wasi_errno_t error =
      __wasi_path_filestat_get(fd, lookup_flags, path, &internal_stat);
  if (error != 0) {
      errno = error;
      return -1;
  }
  to_public_stat(&internal_stat, buf);

  return 0;
#endif
}
