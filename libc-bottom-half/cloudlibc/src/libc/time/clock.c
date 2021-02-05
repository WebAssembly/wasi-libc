// Copyright (c) 2015-2016 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <common/time.h>

#include <assert.h>
#include <wasi/api.h>
#include <time.h>

static_assert(CLOCKS_PER_SEC == NSEC_PER_SEC,
              "Timestamp should need no conversion");

clock_t clock(void) {
  __wasi_timestamp_t ts = 0;
  (void)__wasi_clock_time_get(__WASI_CLOCKID_PROCESS_CPUTIME_ID, 0, &ts);
  return ts;
}
