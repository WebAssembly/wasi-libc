#include "test.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <wasi/version.h>

#define TEST(c)                                                                \
  do {                                                                         \
    errno = 0;                                                                 \
    if (!(c))                                                                  \
      t_error("%s failed (errno = %d)\n", #c, errno);                          \
  } while (0)

#ifdef __wasip1__
#define CLOCK CLOCK_REALTIME
#else
#define CLOCK CLOCK_MONOTONIC
#endif

int main(void) {
  // Sleep for a total of 5ms
  long ns_to_sleep = 5E6;

  struct timespec start_time, end_time;
  clock_gettime(CLOCK, &start_time);
  TEST(usleep(ns_to_sleep / 1000) == 0);
  clock_gettime(CLOCK, &end_time);
  TEST(end_time.tv_sec - start_time.tv_sec <= 1);

  long nanoseconds_elapsed = (end_time.tv_sec - start_time.tv_sec) * 1E9 -
                             start_time.tv_nsec + end_time.tv_nsec;

  TEST(nanoseconds_elapsed >= ns_to_sleep);

  return t_status;
}
