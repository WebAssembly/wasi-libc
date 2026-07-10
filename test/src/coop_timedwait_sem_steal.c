#include "test.h"
#include <errno.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdint.h>
#include <time.h>

#define TEST(c)                                                                \
  do {                                                                         \
    if (!(c))                                                                  \
      t_error("%s failed\n", #c);                                              \
  } while (0)

static sem_t sem;

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

static uint64_t monotonic_ns(void) {
  struct timespec ts;
  TEST(clock_gettime(CLOCK_MONOTONIC, &ts) == 0);
  return (uint64_t)ts.tv_sec * 1000000000ull + (uint64_t)ts.tv_nsec;
}

static void *stolen_waiter(void *arg) {
  (void)arg;
  struct timespec deadline = deadline_ns(4 * 1000 * 1000);
  uint64_t start = monotonic_ns();
  errno = 0;
  TEST(sem_timedwait(&sem, &deadline) == -1);
  TEST(errno == ETIMEDOUT);
  uint64_t elapsed = monotonic_ns() - start;
  TEST(elapsed >= 2ull * 1000 * 1000);
  return NULL;
}

static void *successful_waiter(void *arg) {
  (void)arg;
  struct timespec deadline = deadline_ns(10ll * 1000000000);
  TEST(sem_timedwait(&sem, &deadline) == 0);
  return NULL;
}

int main(void) {
  TEST(sem_init(&sem, 0, 0) == 0);

  // Wake-then-steal: the timed waiter is woken by a post but the permit is
  // consumed before it runs, so it must go back to waiting.
  {
    pthread_t t;
    TEST(pthread_create(&t, NULL, stolen_waiter, NULL) == 0);
    for (int i = 0; i < 5; i++)
      TEST(sched_yield() == 0);
    TEST(sem_post(&sem) == 0);
    TEST(sem_trywait(&sem) == 0);
    TEST(pthread_join(t, NULL) == 0);
  }

  // A post which isn't stolen wakes the timed waiter successfully.
  {
    pthread_t t;
    TEST(pthread_create(&t, NULL, successful_waiter, NULL) == 0);
    for (int i = 0; i < 5; i++)
      TEST(sched_yield() == 0);
    TEST(sem_post(&sem) == 0);
    TEST(pthread_join(t, NULL) == 0);
    int value = 42;
    TEST(sem_getvalue(&sem, &value) == 0);
    TEST(value == 0);
  }

  return t_status;
}
