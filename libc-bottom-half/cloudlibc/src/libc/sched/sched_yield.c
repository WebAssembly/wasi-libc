// Copyright (c) 2015-2016 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#ifndef __wasilibc_use_wasip2
#include <wasi/api.h>
#endif
#include <errno.h>
#include <sched.h>

int sched_yield(void) {
#ifndef __wasilibc_use_wasip2
  __wasi_errno_t error = __wasi_sched_yield();
  if (error != 0) {
    errno = error;
    return -1;
  }
#endif
  return 0;
}

