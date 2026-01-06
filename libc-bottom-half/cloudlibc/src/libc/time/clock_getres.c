// Copyright (c) 2015-2016 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <common/clock.h>
#include <common/time.h>
#include <wasi/api.h>
#include <errno.h>
#include <time.h>

int clock_getres(clockid_t clock_id, struct timespec *res) {
#ifdef __wasip2__
  if (res != NULL) {
    if (clock_id == CLOCK_REALTIME) {
        wall_clock_datetime_t time_result;
        wall_clock_resolution(&time_result);
        *res = timestamp_to_timespec(&time_result);
    } else if (clock_id == CLOCK_MONOTONIC) {
        monotonic_clock_duration_t time_result = monotonic_clock_resolution();
        *res = instant_to_timespec(time_result);
    } else {
        errno = EINVAL; // wasip2 only supports wall and monotonic clocks
        return -1;
    }
  }
#else
  __wasi_timestamp_t ts;
  __wasi_errno_t error = __wasi_clock_res_get(clock_id->id, &ts);
  if (error != 0) {
    errno = error;
    return -1;
  }
  *res = timestamp_to_timespec(ts);
#endif
  return 0;
}
