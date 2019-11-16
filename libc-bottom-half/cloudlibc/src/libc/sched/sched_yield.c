// Copyright (c) 2015-2016 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <wasi/api.h>
#include <errno.h>
#include <sched.h>

int sched_yield(void) {
#ifdef __wasilibc_unmodified_upstream
  __wasi_errno_t error = __wasi_thread_yield();
#else
  __wasi_errno_t error = __wasi_sched_yield();
#endif
  if (error != 0) {
    errno = error;
    return -1;
  }
  return 0;
}
