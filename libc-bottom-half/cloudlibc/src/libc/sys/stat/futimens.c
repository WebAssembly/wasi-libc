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

#include "stat_impl.h"

#ifdef __wasilibc_use_wasip2
static void set_atim_tag_from_flags(__wasi_fstflags_t flags, filesystem_new_timestamp_t* timestamp) {
  if (flags & __WASI_FSTFLAGS_ATIM) {
    timestamp->tag = FILESYSTEM_NEW_TIMESTAMP_TIMESTAMP;
  } else if (flags & __WASI_FSTFLAGS_ATIM_NOW) {
    timestamp->tag = FILESYSTEM_NEW_TIMESTAMP_NOW;
  } else {
    timestamp->tag = FILESYSTEM_NEW_TIMESTAMP_NO_CHANGE;
  }
}

static void set_mtim_tag_from_flags(__wasi_fstflags_t flags, filesystem_new_timestamp_t* timestamp) {
  if (flags & __WASI_FSTFLAGS_MTIM) {
    timestamp->tag = FILESYSTEM_NEW_TIMESTAMP_TIMESTAMP;
  } else if (flags & __WASI_FSTFLAGS_MTIM_NOW) {
    timestamp->tag = FILESYSTEM_NEW_TIMESTAMP_NOW;
  } else {
    timestamp->tag = FILESYSTEM_NEW_TIMESTAMP_NO_CHANGE;
  }
}

int futimens(int fd, const struct timespec *times) {
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
  set_atim_tag_from_flags(flags, &new_timestamp_atim);
  new_timestamp_atim.val.timestamp = st_atim;
  filesystem_new_timestamp_t new_timestamp_mtim;
  set_mtim_tag_from_flags(flags, &new_timestamp_mtim);
  new_timestamp_mtim.val.timestamp = st_mtim;

  // Perform system call.
  filesystem_error_code_t error;
  if (!filesystem_method_descriptor_set_times(file_handle,
                                              &new_timestamp_atim,
                                              &new_timestamp_mtim,
                                              &error)) {
    translate_error(error);
    return -1;
  }

  return 0;
}
#else
int futimens(int fd, const struct timespec *times) {
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
  return 0;
}
#endif
