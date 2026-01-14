// Copyright (c) 2015-2016 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <common/clock.h>
#include <common/time.h>
#include <wasi/api.h>
#include <errno.h>
#include <time.h>

int clock_getres(clockid_t clock_id, struct timespec *res) {
#if defined(__wasip1__)
  __wasi_timestamp_t ts;
  __wasi_errno_t error = __wasi_clock_res_get(clock_id->id, &ts);
  if (error != 0) {
    errno = error;
    return -1;
  }
  *res = timestamp_to_timespec(ts);
#elif defined(__wasip2__) || defined(__wasip3__)
  if (res != NULL) {
    if (clock_id == CLOCK_REALTIME) {
        wall_clock_datetime_t time_result;
#ifdef __wasip2__
        wall_clock_resolution(&time_result);
#else // __wasip3__
        wall_clock_get_resolution(&time_result);
#endif
        *res = timestamp_to_timespec(&time_result);
    } else if (clock_id == CLOCK_MONOTONIC) {
#ifdef __wasip2__
        monotonic_clock_duration_t time_result = monotonic_clock_resolution();
#else // __wasip3__
        monotonic_clock_duration_t time_result = monotonic_clock_get_resolution();
#endif
        *res = instant_to_timespec(time_result);
    } else {
        errno = EINVAL; // wasip2/3 only supports wall and monotonic clocks
        return -1;
    }
  }
#else
# error "Unsupported WASI version"
#endif
  return 0;
}
