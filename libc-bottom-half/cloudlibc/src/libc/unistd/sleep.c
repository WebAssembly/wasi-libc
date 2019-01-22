// Copyright (c) 2015 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <time.h>
#include <unistd.h>

unsigned int sleep(unsigned int seconds) {
  struct timespec ts = {.tv_sec = seconds, .tv_nsec = 0};
#ifdef __wasilibc_unmodified_upstream__
  if (clock_nanosleep(CLOCK_REALTIME, 0, &ts) != 0)
#else
  if (clock_nanosleep(CLOCK_REALTIME, 0, &ts, NULL) != 0)
#endif
    return seconds;
  return 0;
}
