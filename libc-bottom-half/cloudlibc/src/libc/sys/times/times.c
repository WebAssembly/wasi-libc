// Copyright (c) 2015-2016 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <common/time.h>

#include <sys/times.h>

#include <assert.h>
#include <wasi.h>

static_assert(CLOCKS_PER_SEC == NSEC_PER_SEC,
              "Timestamp should need no conversion");

clock_t times(struct tms *buffer) {
  // Obtain user time.
  wasi_timestamp_t usertime = 0;
#ifdef __wasilibc_unmodified_upstream__ // bug fix
#else
  (void)
#endif
  wasi_clock_time_get(WASI_CLOCK_PROCESS_CPUTIME_ID, 0, &usertime);
  *buffer = (struct tms){.tms_utime = usertime};

  // Obtain real time.
  wasi_timestamp_t realtime = 0;
#ifdef __wasilibc_unmodified_upstream__ // bug fix
#else
  (void)
#endif
  wasi_clock_time_get(WASI_CLOCK_MONOTONIC, 0, &realtime);
  return realtime;
}
