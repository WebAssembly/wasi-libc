// Copyright (c) 2015-2016 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <cloudabi_syscalls.h>
#include <errno.h>
#include <unistd.h>

int fsync(int fildes) {
  cloudabi_errno_t error = cloudabi_sys_fd_sync(fildes);
  if (error != 0) {
    errno = error == ENOTCAPABLE ? EINVAL : error;
    return -1;
  }
  return 0;
}
