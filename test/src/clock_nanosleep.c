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

static void test_elapsed_not_too_long(
    struct timespec *start_time,
    struct timespec *end_time,
    long ns_to_sleep)
{
    TEST(end_time->tv_sec - start_time->tv_sec <= 1);

    long nanoseconds_elapsed = (end_time->tv_sec - start_time->tv_sec) * 1E9
        - start_time->tv_nsec
        + end_time->tv_nsec;

    // Test that the difference between the requested amount of sleep
    // and the actual elapsed time is within an acceptable margin
    double difference = abs(nanoseconds_elapsed - ns_to_sleep)
                        / ns_to_sleep;

    // Allow the actual sleep time to be twice as much as the requested time
    TEST(difference <= 1);
}

int main(void)
{
    // Get the clock resolution
    struct timespec clock_resolution;
    clock_getres(CLOCK_MONOTONIC, &clock_resolution);
    // Not much point running the test if the resolution is >= 1 sec
    TEST(clock_resolution.tv_sec==0);


    // Sleep for a total of 5ms
    long ns_to_sleep = 5E6;
    struct timespec time_to_sleep = { .tv_sec = 0, .tv_nsec = ns_to_sleep };

    struct timespec start_time, end_time;

    clock_gettime(CLOCK_MONOTONIC, &start_time);
    TEST(clock_nanosleep(CLOCK_MONOTONIC, 0, &time_to_sleep, NULL)==0);
    clock_gettime(CLOCK_MONOTONIC, &end_time);

    test_elapsed_not_too_long(&start_time, &end_time, ns_to_sleep);

    // Sleep for another 50ms to get us pretty far from 5ms from the start of
    // this process to increase the chance the next test fails if it's not
    // implemented correctly.
    time_to_sleep.tv_nsec = 50E6;
    TEST(clock_nanosleep(CLOCK_MONOTONIC, 0, &time_to_sleep, NULL)==0);

    clock_gettime(CLOCK_MONOTONIC, &time_to_sleep);
    time_to_sleep.tv_nsec += ns_to_sleep;
    if (time_to_sleep.tv_nsec >= 1E9) {
        time_to_sleep.tv_sec += 1;
        time_to_sleep.tv_nsec -= 1E9;
    }
    clock_gettime(CLOCK_MONOTONIC, &start_time);
    TEST(clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &time_to_sleep, NULL)==0);
    clock_gettime(CLOCK_MONOTONIC, &end_time);

    test_elapsed_not_too_long(&start_time, &end_time, ns_to_sleep);

    return t_status;
}
