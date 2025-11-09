#include <sys/time.h>
#include <time.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include "test.h"

#define TEST(c) do { \
	errno = 0; \
	if (!(c)) \
		t_error("%s failed (errno = %d)\n", #c, errno); \
} while(0)

int main(void)
{
    // Get the clock resolution
    struct timespec clock_resolution;
    clock_getres(CLOCK_MONOTONIC, &clock_resolution);
    // Not much point running the test if the resolution is >= 1 sec
    TEST(clock_resolution.tv_sec==0);

    // Adjust this number if necessary
    // In wasip2 we don't get accurate results for sleeping < 1 ms

    struct timespec time_to_sleep = { .tv_sec = 0,
                                      .tv_nsec = 1E6 * clock_resolution.tv_nsec };

    struct timespec start_time, end_time;
    clock_gettime(CLOCK_MONOTONIC, &start_time);

    TEST(clock_nanosleep(CLOCK_MONOTONIC, 0, &time_to_sleep, NULL)==0);
    clock_gettime(CLOCK_MONOTONIC, &end_time);
    TEST(end_time.tv_sec - start_time.tv_sec <= 1);

    long nanoseconds_elapsed = (end_time.tv_sec - start_time.tv_sec)
        - start_time.tv_nsec
        + end_time.tv_nsec;

    // Test that the difference between the requested amount of sleep
    // and the actual elapsed time is within an acceptable margin
    double difference = abs(nanoseconds_elapsed - time_to_sleep.tv_nsec)
                        / time_to_sleep.tv_nsec;

    // Allow the actual sleep time to be twice as much as the requested time
    TEST(difference <= 1);

    return t_status;
}
