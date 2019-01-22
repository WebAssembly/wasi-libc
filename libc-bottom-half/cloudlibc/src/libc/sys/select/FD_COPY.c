// Copyright (c) 2015 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <sys/select.h>

#ifndef FD_COPY
#error "FD_COPY is supposed to be a macro as well"
#endif

// clang-format off
void (FD_COPY)(const fd_set *restrict from, fd_set *restrict to) {
  FD_COPY(from, to);
}
