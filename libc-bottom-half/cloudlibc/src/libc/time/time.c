// Copyright (c) 2015-2016 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <common/time.h>

#ifdef __wasilibc_use_wasip2
#include <wasi/wasip2.h>
#else
#include <wasi/api.h>
#endif
#include <time.h>

time_t time(time_t *tloc) {
#ifdef __wasilibc_use_wasip2
  wall_clock_datetime_t res;
  wall_clock_now(&res);
  uint64_t ts = (res.seconds * NSEC_PER_SEC) + res.nanoseconds;
#else
  __wasi_timestamp_t ts = 0;
  (void)__wasi_clock_time_get(__WASI_CLOCKID_REALTIME, NSEC_PER_SEC, &ts);
#endif
  if (tloc != NULL)
    *tloc = ts / NSEC_PER_SEC;
  return ts / NSEC_PER_SEC;
}
