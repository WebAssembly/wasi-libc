// Copyright (c) 2015-2016 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <sys/mman.h>

#include <assert.h>
#include <cloudabi_syscalls.h>
#include <errno.h>

static_assert(MS_ASYNC == CLOUDABI_MS_ASYNC, "Value mismatch");
static_assert(MS_INVALIDATE == CLOUDABI_MS_INVALIDATE, "Value mismatch");
static_assert(MS_SYNC == CLOUDABI_MS_SYNC, "Value mismatch");

int msync(void *addr, size_t len, int flags) {
  cloudabi_errno_t error = cloudabi_sys_mem_sync(addr, len, flags);
  if (error != 0) {
    errno = error;
    return -1;
  }
  return 0;
}
