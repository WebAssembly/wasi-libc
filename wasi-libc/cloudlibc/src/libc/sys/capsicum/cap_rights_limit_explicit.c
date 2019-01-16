// Copyright (c) 2015-2016 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <sys/capsicum.h>

#include <cloudabi_syscalls.h>
#include <errno.h>

int cap_rights_limit_explicit(int fildes, const cap_rights_t *base,
                              const cap_rights_t *inheriting) {
  cloudabi_fdstat_t fsb = {
      .fs_rights_base = base->__value,
      .fs_rights_inheriting = inheriting->__value,
  };
  cloudabi_errno_t error =
      cloudabi_sys_fd_stat_put(fildes, &fsb, CLOUDABI_FDSTAT_RIGHTS);
  if (error != 0) {
    errno = error;
    return -1;
  }
  return 0;
}
