// Copyright (c) 2015-2016 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <common/time.h>

#include <sys/time.h>

#include <cloudabi_syscalls.h>

int gettimeofday(struct timeval *restrict tp, ...) {
  cloudabi_timestamp_t ts = 0;
  cloudabi_sys_clock_time_get(CLOUDABI_CLOCK_REALTIME, 1000, &ts);
  *tp = timestamp_to_timeval(ts);
  return 0;
}
