// Copyright (c) 2015-2016 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <cloudabi_syscalls.h>
#include <errno.h>
#include <unistd.h>

int dup(int fildes) {
  cloudabi_fd_t newfd;
  cloudabi_errno_t error = cloudabi_sys_fd_dup(fildes, &newfd);
  if (error != 0) {
    errno = error;
    return -1;
  }
  return newfd;
}
