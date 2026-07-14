#include "test.h"
#include <errno.h>
#include <pthread.h>
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

static void *contender(void *arg) {
  (void)arg;

  // Deadline 2ms from now.
  struct timespec deadline;
  TEST(clock_gettime(CLOCK_REALTIME, &deadline) == 0);
  deadline.tv_nsec += 2 * 1000 * 1000;
  if (deadline.tv_nsec >= 1000000000) {
    deadline.tv_nsec -= 1000000000;
    deadline.tv_sec += 1;
  }

  uint64_t start = monotonic_ns();
  TEST(pthread_mutex_timedlock(&lock, &deadline) == ETIMEDOUT);
  uint64_t elapsed = monotonic_ns() - start;

  // The wait should be ~2ms: long enough to prove the deadline was honored
  // rather than reported expired immediately.
  TEST(elapsed >= 1ull * 1000 * 1000);
  return NULL;
}

int main(void) {
  pthread_t t;
  TEST(pthread_mutex_lock(&lock) == 0);
  TEST(pthread_create(&t, NULL, contender, NULL) == 0);
  TEST(pthread_join(t, NULL) == 0);
  TEST(pthread_mutex_unlock(&lock) == 0);
  return t_status;
}
