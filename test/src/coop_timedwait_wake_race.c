#include "test.h"
#include <errno.h>
#include <pthread.h>
#include <sched.h>
#include <stdint.h>
#include <time.h>

#define TEST(c)                                                                \
  do {                                                                         \
    if (!(c))                                                                  \
      t_error("%s failed\n", #c);                                              \
  } while (0)

static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

static uint64_t monotonic_ns(void) {
  struct timespec ts;
  TEST(clock_gettime(CLOCK_MONOTONIC, &ts) == 0);
  return (uint64_t)ts.tv_sec * 1000000000ull + (uint64_t)ts.tv_nsec;
}

// Waits for 50us for the lock, and then unlocks if it we happened to get it.
// Note that the precise timing of this test is a bit racy, and the goal is to
// surface a possible race where a timed lock is ready for a timeout and
// notified that it's ready to get the lock. That'll only happen sometimes
// though.
static void *waiter(void *arg) {
  (void)arg;
  struct timespec timeout;
  TEST(clock_gettime(CLOCK_REALTIME, &timeout) == 0);
  timeout.tv_nsec += 50 * 1000;
  if (timeout.tv_nsec >= 1000000000) {
    timeout.tv_nsec -= 1000000000;
    timeout.tv_sec += 1;
  }

  int rc = pthread_mutex_timedlock(&lock, &timeout);
  if (rc == 0)
    TEST(pthread_mutex_unlock(&lock) == 0);
  return NULL;
}

int main(void) {
  for (int i = 0; i < 20; i++) {
    pthread_t t;
    TEST(pthread_mutex_lock(&lock) == 0);
    TEST(pthread_create(&t, NULL, waiter, NULL) == 0);

    // Let the waiter run until it blocks with its 50us timeout armed.
    for (int j = 0; j < 5; j++)
      TEST(sched_yield() == 0);

    // Busy-wait well past the waiter's timeout without yielding so the
    // waiter stays suspended while its timer fires. Or at least that's' the
    // idea, trying to make that scenario more likely with this loop.
    uint64_t start = monotonic_ns();
    while (monotonic_ns() - start < 500 * 1000) {
      // ...
    }

    // Release the lock which will in theory force the race in the waiter where
    // a timeout is expired and a manual signal was sent (from this unlock).
    TEST(pthread_mutex_unlock(&lock) == 0);
    TEST(pthread_join(t, NULL) == 0);
  }
  return t_status;
}
