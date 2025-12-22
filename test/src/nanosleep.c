#include "test.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>

#define TEST(c)                                                                \
  do {                                                                         \
    errno = 0;                                                                 \
    if (!(c))                                                                  \
      t_error("%s failed (errno = %d)\n", #c, errno);                          \
  } while (0)

int main(void) {
  // Sleep for a total of 5ms
  long ns_to_sleep = 5E6;

  struct timespec start_time, end_time;
  clock_gettime(CLOCK_MONOTONIC, &start_time);
  struct timespec dur;
  dur.tv_sec = 0;
  dur.tv_nsec = ns_to_sleep;
  TEST(nanosleep(&dur, NULL) == 0);
  clock_gettime(CLOCK_MONOTONIC, &end_time);
  TEST(end_time.tv_sec - start_time.tv_sec <= 1);

  long nanoseconds_elapsed = (end_time.tv_sec - start_time.tv_sec) * 1E9 -
                             start_time.tv_nsec + end_time.tv_nsec;

  // Test that the difference between the requested amount of sleep
  // and the actual elapsed time is within an acceptable margin
  double difference = abs(nanoseconds_elapsed - ns_to_sleep) / ns_to_sleep;

  // Allow the actual sleep time to be twice as much as the requested time
  TEST(difference <= 1);

  return t_status;
}
