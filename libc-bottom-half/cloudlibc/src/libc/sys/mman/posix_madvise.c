// Copyright (c) 2015-2016 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <sys/mman.h>

#include <assert.h>
#include <wasi.h>
#include <errno.h>

static_assert(POSIX_MADV_DONTNEED == WASI_ADVICE_DONTNEED,
              "Value mismatch");
static_assert(POSIX_MADV_NORMAL == WASI_ADVICE_NORMAL, "Value mismatch");
static_assert(POSIX_MADV_RANDOM == WASI_ADVICE_RANDOM, "Value mismatch");
static_assert(POSIX_MADV_SEQUENTIAL == WASI_ADVICE_SEQUENTIAL,
              "Value mismatch");
static_assert(POSIX_MADV_WILLNEED == WASI_ADVICE_WILLNEED,
              "Value mismatch");

int posix_madvise(void *addr, size_t len, int advice) {
  return wasi_mem_advise(addr, len, advice);
}
