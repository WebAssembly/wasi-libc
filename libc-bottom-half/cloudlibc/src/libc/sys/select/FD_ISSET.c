// Copyright (c) 2015 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <sys/select.h>

#ifndef FD_ISSET
#error "FD_ISSET is supposed to be a macro as well"
#endif

// clang-format off
int (FD_ISSET)(int fd, const fd_set *fd_set) {
  return FD_ISSET(fd, fd_set);
}
