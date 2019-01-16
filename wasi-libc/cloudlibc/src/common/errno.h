// Copyright (c) 2015-2016 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#ifndef COMMON_ERRNO_H
#define COMMON_ERRNO_H

#include <cloudabi_syscalls.h>

// Translates ENOTCAPABLE to ENOTDIR if not a directory.
static inline cloudabi_errno_t errno_fixup_directory(cloudabi_fd_t fd,
                                                     cloudabi_errno_t error) {
  if (error == CLOUDABI_ENOTCAPABLE) {
    cloudabi_fdstat_t fds;
    if (cloudabi_sys_fd_stat_get(fd, &fds) == 0 &&
        fds.fs_filetype != CLOUDABI_FILETYPE_DIRECTORY)
      return CLOUDABI_ENOTDIR;
  }
  return error;
}

// Translates ENOTCAPABLE to EBADF if a regular file or EACCES otherwise.
static inline cloudabi_errno_t errno_fixup_executable(cloudabi_fd_t fd,
                                                      cloudabi_errno_t error) {
  if (error == CLOUDABI_ENOTCAPABLE) {
    cloudabi_fdstat_t fds;
    if (cloudabi_sys_fd_stat_get(fd, &fds) == 0)
      return fds.fs_filetype == CLOUDABI_FILETYPE_REGULAR_FILE
                 ? CLOUDABI_EBADF
                 : CLOUDABI_EACCES;
  }
  return error;
}

// Translates ENOTCAPABLE to EINVAL if not a process.
static inline cloudabi_errno_t errno_fixup_process(cloudabi_fd_t fd,
                                                   cloudabi_errno_t error) {
  if (error == CLOUDABI_ENOTCAPABLE) {
    cloudabi_fdstat_t fds;
    if (cloudabi_sys_fd_stat_get(fd, &fds) == 0 &&
        fds.fs_filetype != CLOUDABI_FILETYPE_PROCESS)
      return CLOUDABI_EINVAL;
  }
  return error;
}

// Translates ENOTCAPABLE to ENOTSOCK if not a socket.
static inline cloudabi_errno_t errno_fixup_socket(cloudabi_fd_t fd,
                                                  cloudabi_errno_t error) {
  if (error == CLOUDABI_ENOTCAPABLE) {
    cloudabi_fdstat_t fds;
    if (cloudabi_sys_fd_stat_get(fd, &fds) == 0 &&
        (fds.fs_filetype & 0xf0) != 0x80)
      return CLOUDABI_ENOTSOCK;
  }
  return error;
}

#endif
