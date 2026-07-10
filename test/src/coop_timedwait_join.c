#define _GNU_SOURCE
#include "test.h"
#include <errno.h>
#include <pthread.h>
#include <stdint.h>
#include <time.h>

#define TEST(c)                                                                \
  do {                                                                         \
    if (!(c))                                                                  \
      t_error("%s failed\n", #c);                                              \
  } while (0)

static struct timespec deadline_ns(int64_t ns) {
  struct timespec ts;
  TEST(clock_gettime(CLOCK_REALTIME, &ts) == 0);
  ts.tv_sec += ns / 1000000000;
  ts.tv_nsec += ns % 1000000000;
  if (ts.tv_nsec >= 1000000000) {
    ts.tv_nsec -= 1000000000;
    ts.tv_sec += 1;
  }
  return ts;
}

static void *sleepy(void *arg) {
  (void)arg;
  struct timespec d = {.tv_sec = 0, .tv_nsec = 3 * 1000 * 1000};
  TEST(nanosleep(&d, NULL) == 0);
  return (void *)(intptr_t)42;
}

static void *quick(void *arg) {
  (void)arg;
  return (void *)(intptr_t)42;
}

int main(void) {
  pthread_t t;
  void *res = NULL;
  TEST(pthread_create(&t, NULL, sleepy, NULL) == 0);

  // Wait for less than the sleep is happening, but if CI is super slow just let
  // this pass anyway.
  struct timespec deadline = deadline_ns(1 * 1000 * 1000);
  if (pthread_timedjoin_np(t, &res, &deadline) == ETIMEDOUT) {
    deadline = deadline_ns(100ll * 1000000000);
    TEST(pthread_timedjoin_np(t, &res, &deadline) == 0);
  }
  TEST((intptr_t)res == 42);

  // Timed join of an already-exited thread returns immediately.
  TEST(pthread_create(&t, NULL, quick, NULL) == 0);
  for (int i = 0; i < 5; i++)
    TEST(sched_yield() == 0);
  res = NULL;
  deadline = deadline_ns(0);
  TEST(pthread_timedjoin_np(t, &res, &deadline) == 0);
  TEST((intptr_t)res == 42);

  return t_status;
}
