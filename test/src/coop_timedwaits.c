#include "test.h"
#include <errno.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

#define TEST(c)                                                                \
  do {                                                                         \
    if (!(c))                                                                  \
      t_error("%s failed\n", #c);                                              \
  } while (0)

static pthread_mutex_t lock;

// Returns an absolute CLOCK_REALTIME deadline `ns` nanoseconds from now, as
// POSIX specifies for `pthread_mutex_timedlock` and friends.
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

static void *assert_lock_times_out(void *arg) {
  (void)arg;
  // A deadline in the past times out immediately.
  struct timespec timeout = {.tv_sec = 0, .tv_nsec = 0};
  TEST(pthread_mutex_timedlock(&lock, &timeout) == ETIMEDOUT);
  // As does a deadline in the very near future.
  timeout = deadline_ns(1000);
  TEST(pthread_mutex_timedlock(&lock, &timeout) == ETIMEDOUT);
  return NULL;
}

static void *assert_lock_works(void *arg) {
  (void)arg;
  struct timespec timeout = deadline_ns(100ll * 1000000000);
  TEST(pthread_mutex_timedlock(&lock, &timeout) == 0);
  TEST(pthread_mutex_unlock(&lock) == 0);
  return NULL;
}

int main(void) {
  TEST(pthread_mutex_init(&lock, NULL) == 0);

  // Uncontended lock acquisition works.
  struct timespec timeout = deadline_ns(100ll * 1000000000);
  TEST(pthread_mutex_timedlock(&lock, &timeout) == 0);
  TEST(pthread_mutex_unlock(&lock) == 0);

  // Timeout does indeed time out.
  {
    pthread_t child_thread;
    TEST(pthread_mutex_lock(&lock) == 0);
    TEST(pthread_create(&child_thread, NULL, assert_lock_times_out, NULL) == 0);
    TEST(pthread_join(child_thread, NULL) == 0);
    TEST(pthread_mutex_unlock(&lock) == 0);
  }

  // Other threads are woken when the lock is unlocked.
  {
    pthread_t child_thread;
    TEST(pthread_mutex_lock(&lock) == 0);
    TEST(pthread_create(&child_thread, NULL, assert_lock_works, NULL) == 0);
    for (int i = 0; i < 5; i++)
      TEST(sched_yield() == 0);
    TEST(pthread_mutex_unlock(&lock) == 0);
    TEST(pthread_join(child_thread, NULL) == 0);
    TEST(pthread_mutex_lock(&lock) == 0);
    TEST(pthread_mutex_unlock(&lock) == 0);
  }

  return t_status;
}
