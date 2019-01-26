// Copyright (c) 2015-2016 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <assert.h>
#include <wasi.h>
#include <errno.h>
#include <fcntl.h>

static_assert(POSIX_FADV_DONTNEED == WASI_ADVICE_DONTNEED,
              "Value mismatch");
static_assert(POSIX_FADV_NOREUSE == WASI_ADVICE_NOREUSE, "Value mismatch");
static_assert(POSIX_FADV_NORMAL == WASI_ADVICE_NORMAL, "Value mismatch");
static_assert(POSIX_FADV_RANDOM == WASI_ADVICE_RANDOM, "Value mismatch");
static_assert(POSIX_FADV_SEQUENTIAL == WASI_ADVICE_SEQUENTIAL,
              "Value mismatch");
static_assert(POSIX_FADV_WILLNEED == WASI_ADVICE_WILLNEED,
              "Value mismatch");

int posix_fadvise(int fd, off_t offset, off_t len, int advice) {
  if (offset < 0 || len < 0)
    return EINVAL;
  return wasi_file_advise(fd, offset, len, advice);
}
