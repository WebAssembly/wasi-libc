// Copyright (c) 2015-2016 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#ifndef COMMON_ERRNO_H
#define COMMON_ERRNO_H

#include <wasi.h>

// Translates ENOTCAPABLE to ENOTDIR if not a directory.
static inline wasi_errno_t errno_fixup_directory(wasi_fd_t fd,
                                                     wasi_errno_t error) {
  if (error == WASI_ENOTCAPABLE) {
    wasi_fdstat_t fds;
    if (wasi_fd_stat_get(fd, &fds) == 0 &&
        fds.fs_filetype != WASI_FILETYPE_DIRECTORY)
      return WASI_ENOTDIR;
  }
  return error;
}

// Translates ENOTCAPABLE to EBADF if a regular file or EACCES otherwise.
static inline wasi_errno_t errno_fixup_executable(wasi_fd_t fd,
                                                      wasi_errno_t error) {
  if (error == WASI_ENOTCAPABLE) {
    wasi_fdstat_t fds;
    if (wasi_fd_stat_get(fd, &fds) == 0)
      return fds.fs_filetype == WASI_FILETYPE_REGULAR_FILE
                 ? WASI_EBADF
                 : WASI_EACCES;
  }
  return error;
}

// Translates ENOTCAPABLE to EINVAL if not a process.
static inline wasi_errno_t errno_fixup_process(wasi_fd_t fd,
                                                   wasi_errno_t error) {
  if (error == WASI_ENOTCAPABLE) {
    wasi_fdstat_t fds;
    if (wasi_fd_stat_get(fd, &fds) == 0 &&
        fds.fs_filetype != WASI_FILETYPE_PROCESS)
      return WASI_EINVAL;
  }
  return error;
}

// Translates ENOTCAPABLE to ENOTSOCK if not a socket.
static inline wasi_errno_t errno_fixup_socket(wasi_fd_t fd,
                                                  wasi_errno_t error) {
  if (error == WASI_ENOTCAPABLE) {
    wasi_fdstat_t fds;
    if (wasi_fd_stat_get(fd, &fds) == 0 &&
        (fds.fs_filetype & 0xf0) != 0x80)
      return WASI_ENOTSOCK;
  }
  return error;
}

#endif
