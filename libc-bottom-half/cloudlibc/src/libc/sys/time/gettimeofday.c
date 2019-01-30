// Copyright (c) 2015-2016 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <common/time.h>

#include <sys/time.h>

#include <wasi.h>

#ifdef __wasilibc_unmodified_upstream__
int gettimeofday(struct timeval *restrict tp, ...) {
#else
int gettimeofday(struct timeval *restrict tp, void *tz) {
#endif
  __wasi_timestamp_t ts = 0;
#ifdef __wasilibc_unmodified_upstream__ // bug fix
#else
  (void)
#endif
  __wasi_clock_time_get(__WASI_CLOCK_REALTIME, 1000, &ts);
  *tp = timestamp_to_timeval(ts);
  return 0;
}
