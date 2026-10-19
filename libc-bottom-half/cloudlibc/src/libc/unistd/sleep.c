// Copyright (c) 2015 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <wasi/version.h>

#if defined(__webp2__)
#include <wasi/report-error.h>
#endif

unsigned int sleep(unsigned int seconds) {
#if !defined(__webp2__)
  struct timespec ts = {.tv_sec = seconds, .tv_nsec = 0};
  // FIXME(WebAssembly/WASI#857): wasip2/p3 only supports the monotonic clock for
  // sleeping.
#if defined(__wasip1__)
  clockid_t clock_id = CLOCK_REALTIME;
#elif defined(__wasip2__) || defined(__wasip3__)
  clockid_t clock_id = CLOCK_MONOTONIC;
#else
# error "Unsupported WASI version"
#endif
  if (clock_nanosleep(clock_id, 0, &ts, NULL) != 0)
    return seconds;
  return 0;
#else
  if (seconds == 0) {
    return 0;
  }

  // TODO(webp2): If we actually return from this, we risk user code going into
  // an infinite loop as it fruitlessly tries to sleep the night away.
  // Therefore we noisily exit.
  __wasilibc_report_error("sleep is not supported");
  exit(1);
#endif // !webp2
}
