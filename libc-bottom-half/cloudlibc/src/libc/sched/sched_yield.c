// Copyright (c) 2015-2016 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <wasi/api.h>
#include <errno.h>
#include <sched.h>

int sched_yield(void) {
#ifdef __wasi_cooperative_threads__
  #ifdef __wasip3__
    wasip3_thread_yield();
    return 0;
  #else
    #error "Unknown WASI version"
  #endif
#elif defined(__wasip1__)
#ifdef __wasip1__
  __wasi_errno_t error = __wasi_sched_yield();
  if (error != 0) {
    errno = error;
    return -1;
  }
#endif
  return 0;
}

