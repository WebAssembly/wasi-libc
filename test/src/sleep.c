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
  // Sleep for a total of 2 seconds
  unsigned int seconds_to_sleep = 2;

  struct timespec start_time, end_time;
  clock_gettime(CLOCK, &start_time);
  TEST(sleep(seconds_to_sleep) == 0);
  clock_gettime(CLOCK, &end_time);
  TEST(end_time.tv_sec - start_time.tv_sec <= 3);

  long nanoseconds_elapsed = (end_time.tv_sec - start_time.tv_sec) * 1E9 -
                             start_time.tv_nsec + end_time.tv_nsec;

  TEST(nanoseconds_elapsed >= seconds_to_sleep * 1E9);

  return t_status;
}
