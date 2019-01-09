// Copyright (c) 2015-2016 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <cloudabi_syscalls.h>
#include <errno.h>
#include <unistd.h>

ssize_t pread(int fildes, void *buf, size_t nbyte, off_t offset) {
  if (offset < 0) {
    errno = EINVAL;
    return -1;
  }
  cloudabi_iovec_t iov = {.buf = buf, .buf_len = nbyte};
  size_t bytes_read;
  cloudabi_errno_t error =
      cloudabi_sys_fd_pread(fildes, &iov, 1, offset, &bytes_read);
  if (error != 0) {
    cloudabi_fdstat_t fds;
    if (error == ENOTCAPABLE && cloudabi_sys_fd_stat_get(fildes, &fds) == 0) {
      // Determine why we got ENOTCAPABLE.
      if ((fds.fs_rights_base & CLOUDABI_RIGHT_FD_READ) == 0)
        error = EBADF;
      else
        error = ESPIPE;
    }
    errno = error;
    return -1;
  }
  return bytes_read;
}
