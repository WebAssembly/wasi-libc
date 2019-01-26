// Copyright (c) 2015-2016 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <sys/mman.h>

#include <wasi.h>
#include <errno.h>

int munmap(void *addr, size_t len) {
  wasi_errno_t error = wasi_mem_unmap(addr, len);
  if (error != 0) {
    errno = error;
    return -1;
  }
  return 0;
}
