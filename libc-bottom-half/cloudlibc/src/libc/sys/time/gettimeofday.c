// Copyright (c) 2015-2016 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <common/time.h>

#include <sys/time.h>

#ifdef __wasilibc_use_wasip2
#include <wasi/wasip2.h>
#else
#include <wasi/api.h>
#endif

int gettimeofday(struct timeval *restrict tp, void *tz) {
  if (tp != NULL) {
#ifdef __wasilibc_use_wasip2
    wall_clock_datetime_t time_result;
    wall_clock_now(&time_result);
    *tp = timestamp_to_timeval(&time_result);
#else
    __wasi_timestamp_t ts = 0;
    (void)__wasi_clock_time_get(__WASI_CLOCKID_REALTIME, 1000, &ts);
    *tp = timestamp_to_timeval(ts);
#endif
  }
  return 0;
}
