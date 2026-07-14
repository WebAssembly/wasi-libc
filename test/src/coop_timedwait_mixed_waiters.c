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

static pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t c = PTHREAD_COND_INITIALIZER;
static int go = 0;
static int wakeups = 0;
static int timeouts = 0;

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

static void *indefinite_waiter(void *arg) {
  (void)arg;
  TEST(pthread_mutex_lock(&m) == 0);
  while (!go)
    TEST(pthread_cond_wait(&c, &m) == 0);
  wakeups++;
  TEST(pthread_mutex_unlock(&m) == 0);
  return NULL;
}

static void *timed_waiter_expires(void *arg) {
  (void)arg;
  struct timespec deadline = deadline_ns(1 * 1000 * 1000);
  TEST(pthread_mutex_lock(&m) == 0);
  int rc = 0;
  while (!go && rc == 0)
    rc = pthread_cond_timedwait(&c, &m, &deadline);
  TEST(rc == ETIMEDOUT);
  TEST(!go);
  timeouts++;
  TEST(pthread_mutex_unlock(&m) == 0);
  return NULL;
}

static void *timed_waiter_signaled(void *arg) {
  (void)arg;
  struct timespec deadline = deadline_ns(10ll * 1000000000);
  TEST(pthread_mutex_lock(&m) == 0);
  while (!go)
    TEST(pthread_cond_timedwait(&c, &m, &deadline) == 0);
  wakeups++;
  TEST(pthread_mutex_unlock(&m) == 0);
  return NULL;
}

int main(void) {
  // Waitlist is [indefinite, timed, indefinite] (each thread blocks
  // before the next is created, and insertion prepends), so the timed waiter
  // unlinks itself from the middle of the list when it expires.
  {
    pthread_t a, b, d;
    TEST(pthread_create(&a, NULL, indefinite_waiter, NULL) == 0);
    for (int i = 0; i < 3; i++)
      TEST(sched_yield() == 0);
    TEST(pthread_create(&b, NULL, timed_waiter_expires, NULL) == 0);
    for (int i = 0; i < 3; i++)
      TEST(sched_yield() == 0);
    TEST(pthread_create(&d, NULL, indefinite_waiter, NULL) == 0);
    for (int i = 0; i < 3; i++)
      TEST(sched_yield() == 0);

    // Wait for the timed waiter to expire and remove itself from the middle
    // of the list; joining it blocks only until its ~1ms deadline passes.
    TEST(pthread_join(b, NULL) == 0);
    TEST(timeouts == 1);

    // The two indefinite waiters remain on the list and wake on broadcast.
    TEST(pthread_mutex_lock(&m) == 0);
    go = 1;
    TEST(pthread_cond_broadcast(&c) == 0);
    TEST(pthread_mutex_unlock(&m) == 0);
    TEST(pthread_join(a, NULL) == 0);
    TEST(pthread_join(d, NULL) == 0);
    TEST(wakeups == 2);
  }

  // Broadcast over a mixed list of one indefinite and one timed waiter wakes
  // both.
  {
    go = 0;
    wakeups = 0;
    pthread_t a, b;
    TEST(pthread_create(&a, NULL, indefinite_waiter, NULL) == 0);
    for (int i = 0; i < 3; i++)
      TEST(sched_yield() == 0);
    TEST(pthread_create(&b, NULL, timed_waiter_signaled, NULL) == 0);
    for (int i = 0; i < 3; i++)
      TEST(sched_yield() == 0);

    TEST(pthread_mutex_lock(&m) == 0);
    go = 1;
    TEST(pthread_cond_broadcast(&c) == 0);
    TEST(pthread_mutex_unlock(&m) == 0);
    TEST(pthread_join(a, NULL) == 0);
    TEST(pthread_join(b, NULL) == 0);
    TEST(wakeups == 2);
  }

  return t_status;
}
