// Copyright (c) 2015-2016 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <sys/mman.h>

#include <assert.h>
#include <cloudabi_syscalls.h>
#include <errno.h>

static_assert(MAP_ANON == CLOUDABI_MAP_ANON, "Value mismatch");
static_assert(MAP_FIXED == CLOUDABI_MAP_FIXED, "Value mismatch");
static_assert(MAP_PRIVATE == CLOUDABI_MAP_PRIVATE, "Value mismatch");
static_assert(MAP_SHARED == CLOUDABI_MAP_SHARED, "Value mismatch");

static_assert(PROT_EXEC == CLOUDABI_PROT_EXEC, "Value mismatch");
static_assert(PROT_WRITE == CLOUDABI_PROT_WRITE, "Value mismatch");
static_assert(PROT_READ == CLOUDABI_PROT_READ, "Value mismatch");

void *mmap(void *addr, size_t len, int prot, int flags, int fildes, off_t off) {
  void *mapping;
  cloudabi_errno_t error =
      cloudabi_sys_mem_map(addr, len, prot, flags, fildes, off, &mapping);
  if (error != 0) {
    errno = error;
    return MAP_FAILED;
  }
  return mapping;
}
