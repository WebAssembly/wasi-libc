// Copyright (c) 2015 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <time.h>
#include <unistd.h>

unsigned int sleep(unsigned int seconds) {
  struct timespec ts = {.tv_sec = seconds, .tv_nsec = 0};
  // FIXME(WebAssembly/WASI#857): wasip2/p3 only supports the monotonic clock for
  // sleeping.
  #if defined(__wasip1__)
  clockid_t clock_id = CLOCK_REALTIME;
  #elif defined(__wasip2__) || defined(__wasip3__)
  clockid_t clock_id = CLOCK_MONOTONIC;
  #else
  #error "Unsupported WASI version"
  #endif
  if (clock_nanosleep(clock_id, 0, &ts, NULL) != 0)
    return seconds;
  return 0;
}
