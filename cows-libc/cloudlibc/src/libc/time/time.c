// Copyright (c) 2015-2016 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <common/time.h>

#include <cloudabi_syscalls.h>
#include <time.h>

time_t time(time_t *tloc) {
  cloudabi_timestamp_t ts = 0;
  cloudabi_sys_clock_time_get(CLOUDABI_CLOCK_REALTIME, NSEC_PER_SEC, &ts);
  if (tloc != NULL)
    *tloc = ts / NSEC_PER_SEC;
  return ts / NSEC_PER_SEC;
}
