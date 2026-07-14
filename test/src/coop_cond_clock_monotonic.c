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
static pthread_cond_t c;
static int go = 0;

static uint64_t monotonic_ns(void) {
  struct timespec ts;
  TEST(clock_gettime(CLOCK_MONOTONIC, &ts) == 0);
  return (uint64_t)ts.tv_sec * 1000000000ull + (uint64_t)ts.tv_nsec;
}

static struct timespec monotonic_deadline_ns(int64_t ns) {
  struct timespec ts;
  TEST(clock_gettime(CLOCK_MONOTONIC, &ts) == 0);
  ts.tv_sec += ns / 1000000000;
  ts.tv_nsec += ns % 1000000000;
  if (ts.tv_nsec >= 1000000000) {
    ts.tv_nsec -= 1000000000;
    ts.tv_sec += 1;
  }
  return ts;
}

static void *signaler(void *arg) {
  (void)arg;
  TEST(pthread_mutex_lock(&m) == 0);
  go = 1;
  TEST(pthread_cond_signal(&c) == 0);
  TEST(pthread_mutex_unlock(&m) == 0);
  return NULL;
}

int main(void) {
  pthread_condattr_t a;
  TEST(pthread_condattr_init(&a) == 0);
  TEST(pthread_condattr_setclock(&a, CLOCK_MONOTONIC) == 0);
  TEST(pthread_cond_init(&c, &a) == 0);

  // An unsignaled wait with a monotonic deadline 2ms out must actually wait
  // ~2ms before reporting ETIMEDOUT.
  {
    struct timespec deadline = monotonic_deadline_ns(2 * 1000 * 1000);
    TEST(pthread_mutex_lock(&m) == 0);
    uint64_t start = monotonic_ns();
    int rc = 0;
    while (!go && rc == 0)
      rc = pthread_cond_timedwait(&c, &m, &deadline);
    uint64_t elapsed = monotonic_ns() - start;
    TEST(rc == ETIMEDOUT);
    TEST(elapsed >= 1ull * 1000 * 1000);
    TEST(pthread_mutex_unlock(&m) == 0);
  }

  go = 0;

  // A signaled wait with a monotonic deadline returns 0.
  {
    struct timespec deadline = monotonic_deadline_ns(10ll * 1000000000);
    TEST(pthread_mutex_lock(&m) == 0);
    pthread_t t;
    TEST(pthread_create(&t, NULL, signaler, NULL) == 0);
    while (!go)
      TEST(pthread_cond_timedwait(&c, &m, &deadline) == 0);
    TEST(pthread_mutex_unlock(&m) == 0);
    TEST(pthread_join(t, NULL) == 0);
  }

  return t_status;
}
