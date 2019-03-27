// Copyright (c) 2015 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <errno.h>
#include <threads.h>
#include <time.h>

#ifdef __wasilibc_unmodified_upstream
int nanosleep(const struct timespec *rqtp, ...) {
#else
int nanosleep(const struct timespec *rqtp, struct timespec *rem) {
#endif
#ifdef __wasilibc_unmodified_upstream
  int error = clock_nanosleep(CLOCK_REALTIME, 0, rqtp);
#else
  int error = clock_nanosleep(CLOCK_REALTIME, 0, rqtp, rem);
#endif
  if (error != 0) {
    errno = error;
    return -1;
  }
  return 0;
}

#if defined(__wasilibc_unmodified_upstream) || defined(_REENTRANT)
__strong_reference(nanosleep, thrd_sleep);
#else
#endif
