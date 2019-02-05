// Copyright (c) 2015-2016 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <sys/mman.h>

#include <assert.h>
#include <wasi.h>
#include <errno.h>

static_assert(MAP_ANON == __WASI_MAP_ANON, "Value mismatch");
static_assert(MAP_FIXED == __WASI_MAP_FIXED, "Value mismatch");
static_assert(MAP_PRIVATE == __WASI_MAP_PRIVATE, "Value mismatch");
static_assert(MAP_SHARED == __WASI_MAP_SHARED, "Value mismatch");

static_assert(PROT_EXEC == __WASI_PROT_EXEC, "Value mismatch");
static_assert(PROT_WRITE == __WASI_PROT_WRITE, "Value mismatch");
static_assert(PROT_READ == __WASI_PROT_READ, "Value mismatch");

void *mmap(void *addr, size_t len, int prot, int flags, int fildes, off_t off) {
  void *mapping;
  __wasi_errno_t error =
      __wasi_mem_map(addr, len, prot, flags, fildes, off, &mapping);
  if (error != 0) {
    errno = error;
    return MAP_FAILED;
  }
  return mapping;
}