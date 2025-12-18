// Copyright (c) 2015-2016 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <sys/stat.h>
#include <wasi/api.h>
#include <errno.h>

#ifdef __wasip2__
#include <wasi/file_utils.h>
#include <common/errors.h>
#endif

#include "stat_impl.h"

int futimens(int fd, const struct timespec *times) {
#ifdef __wasip2__
  // Translate the file descriptor to an internal handle
  filesystem_borrow_descriptor_t file_handle;
  if (fd_to_file_handle(fd, &file_handle) < 0)
    return -1;

  // Convert timestamps and extract NOW/OMIT flags.
  filesystem_new_timestamp_t new_timestamp_atim;
  filesystem_new_timestamp_t new_timestamp_mtim;
  if (!utimens_get_timestamps(times, &new_timestamp_atim, &new_timestamp_mtim)) {
    errno = EINVAL;
    return -1;
  }

  // Perform system call.
  filesystem_error_code_t error;
  if (!filesystem_method_descriptor_set_times(file_handle,
                                              &new_timestamp_atim,
                                              &new_timestamp_mtim,
                                              &error)) {
    translate_error(error);
    return -1;
  }

#else
  // Convert timestamps and extract NOW/OMIT flags.
  __wasi_timestamp_t st_atim;
  __wasi_timestamp_t st_mtim;
  __wasi_fstflags_t flags;
  if (!utimens_get_timestamps(times, &st_atim, &st_mtim, &flags)) {
    errno = EINVAL;
    return -1;
  }

  // Perform system call.
  __wasi_errno_t error = __wasi_fd_filestat_set_times(fd, st_atim, st_mtim, flags);
  if (error != 0) {
    errno = error;
    return -1;
  }
#endif

  return 0;
}
