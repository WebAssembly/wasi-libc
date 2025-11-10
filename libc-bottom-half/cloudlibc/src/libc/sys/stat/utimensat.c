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

#include "stat_impl.h"

#ifdef __wasilibc_use_wasip2
int __wasilibc_nocwd_utimensat(int fd, const char *path, const struct timespec times[2],
                               int flag) {
  // Translate the file descriptor to an internal handle
  // Translate the file descriptor to an internal handle
  filesystem_borrow_descriptor_t file_handle;
  if (!fd_to_file_handle_allow_open(fd, &file_handle)) {
    errno = EBADF;
    return -1;
  }

  // Convert timestamps and extract NOW/OMIT flags.
  filesystem_datetime_t st_atim;
  filesystem_datetime_t st_mtim;
  __wasi_fstflags_t flags;
  if (!utimens_get_timestamps(times, &st_atim, &st_mtim, &flags)) {
    errno = EINVAL;
    return -1;
  }

  // Set up filesystem_new_timestamps
  filesystem_new_timestamp_t new_timestamp_atim;
  new_timestamp_atim.tag = FILESYSTEM_NEW_TIMESTAMP_TIMESTAMP;
  new_timestamp_atim.val.timestamp = st_atim;
  filesystem_new_timestamp_t new_timestamp_mtim;
  new_timestamp_mtim.tag = FILESYSTEM_NEW_TIMESTAMP_TIMESTAMP;
  new_timestamp_mtim.val.timestamp = st_mtim;

  // Create lookup properties.
  __wasi_lookupflags_t lookup_flags = 0;
  if ((flag & AT_SYMLINK_NOFOLLOW) == 0)
    lookup_flags |= __WASI_LOOKUPFLAGS_SYMLINK_FOLLOW;

  // Convert the string into a Wasm string
  wasip2_string_t path_wasm_string = wasip2_string_temp(path);

  // Perform system call.
  filesystem_error_code_t error;
  bool set_times_result = filesystem_method_descriptor_set_times_at(file_handle,
                                                                    lookup_flags,
                                                                    &path_wasm_string,
                                                                    &new_timestamp_atim,
                                                                    &new_timestamp_mtim,
                                                                    &error);
  if (!set_times_result) {
    translate_error(error);
    return -1;
  }

  return 0;
}
#else
int __wasilibc_nocwd_utimensat(int fd, const char *path, const struct timespec times[2],
                               int flag) {
  // Convert timestamps and extract NOW/OMIT flags.
  __wasi_timestamp_t st_atim;
  __wasi_timestamp_t st_mtim;
  __wasi_fstflags_t flags;
  if (!utimens_get_timestamps(times, &st_atim, &st_mtim, &flags)) {
    errno = EINVAL;
    return -1;
  }

  // Create lookup properties.
  __wasi_lookupflags_t lookup_flags = 0;
  if ((flag & AT_SYMLINK_NOFOLLOW) == 0)
    lookup_flags |= __WASI_LOOKUPFLAGS_SYMLINK_FOLLOW;

  // Perform system call.
  __wasi_errno_t error =
      __wasi_path_filestat_set_times(fd, lookup_flags, path, st_atim, st_mtim, flags);
  if (error != 0) {
    errno = error;
    return -1;
  }
  return 0;
}
#endif
