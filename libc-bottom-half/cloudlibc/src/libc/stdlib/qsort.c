// Copyright (c) 2015 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <stdlib.h>

#ifndef qsort
#error "qsort is supposed to be a macro as well"
#endif

// clang-format off
void (qsort)(void *base, size_t nel, size_t width,
             int (*compar)(const void *, const void *)) {
  return qsort(base, nel, width, compar);
}
