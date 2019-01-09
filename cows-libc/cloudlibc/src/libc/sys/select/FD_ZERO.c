// Copyright (c) 2015 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <sys/select.h>

#ifndef FD_ZERO
#error "FD_ZERO is supposed to be a macro as well"
#endif

// clang-format off
void (FD_ZERO)(fd_set *fd_set) {
  FD_ZERO(fd_set);
}
