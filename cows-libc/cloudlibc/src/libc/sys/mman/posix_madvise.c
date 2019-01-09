// Copyright (c) 2015-2016 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <sys/mman.h>

#include <assert.h>
#include <cloudabi_syscalls.h>
#include <errno.h>

static_assert(POSIX_MADV_DONTNEED == CLOUDABI_ADVICE_DONTNEED,
              "Value mismatch");
static_assert(POSIX_MADV_NORMAL == CLOUDABI_ADVICE_NORMAL, "Value mismatch");
static_assert(POSIX_MADV_RANDOM == CLOUDABI_ADVICE_RANDOM, "Value mismatch");
static_assert(POSIX_MADV_SEQUENTIAL == CLOUDABI_ADVICE_SEQUENTIAL,
              "Value mismatch");
static_assert(POSIX_MADV_WILLNEED == CLOUDABI_ADVICE_WILLNEED,
              "Value mismatch");

int posix_madvise(void *addr, size_t len, int advice) {
  return cloudabi_sys_mem_advise(addr, len, advice);
}
