// Copyright (c) 2015-2016 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <common/clock.h>
#include <common/time.h>

#include <wasi.h>
#include <errno.h>
#include <time.h>

#ifdef __wasilibc_unmodified_upstream__
int clock_gettime(clockid_t clock_id, struct timespec *tp) {
#else
int __clock_gettime(clockid_t clock_id, struct timespec *tp) {
#endif
  __wasi_timestamp_t ts;
  __wasi_errno_t error = __wasi_clock_time_get(clock_id->id, 1, &ts);
  if (error != 0) {
    errno = error;
    return -1;
  }
  *tp = timestamp_to_timespec(ts);
  return 0;
}
#ifdef __wasilibc_unmodified_upstream__
#else
extern __typeof(__clock_gettime) clock_gettime __attribute__((weak, alias("__clock_gettime")));
#endif
