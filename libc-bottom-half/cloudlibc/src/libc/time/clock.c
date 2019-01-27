// Copyright (c) 2015-2016 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <common/time.h>

#include <assert.h>
#include <wasi.h>
#include <time.h>

static_assert(CLOCKS_PER_SEC == NSEC_PER_SEC,
              "Timestamp should need no conversion");

clock_t clock(void) {
  wasi_timestamp_t ts = 0;
#ifdef __wasilibc_unmodified_upstream__ // bug fix
#else
  (void)
#endif
  wasi_clock_time_get(WASI_CLOCK_PROCESS_CPUTIME_ID, 0, &ts);
  return ts;
}
