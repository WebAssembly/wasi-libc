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

static pthread_rwlock_t rw = PTHREAD_RWLOCK_INITIALIZER;

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

static void *against_writer(void *arg) {
  (void)arg;
  // Writer held: both timed acquisitions time out (~1ms deadlines).
  struct timespec t = deadline_ns(1 * 1000 * 1000);
  TEST(pthread_rwlock_timedrdlock(&rw, &t) == ETIMEDOUT);
  t = deadline_ns(1 * 1000 * 1000);
  TEST(pthread_rwlock_timedwrlock(&rw, &t) == ETIMEDOUT);
  return NULL;
}

static void *against_reader(void *arg) {
  (void)arg;
  // Reader held: another timed read lock succeeds immediately...
  struct timespec t = deadline_ns(10ll * 1000000000);
  TEST(pthread_rwlock_timedrdlock(&rw, &t) == 0);
  TEST(pthread_rwlock_unlock(&rw) == 0);
  // ...but a timed write lock times out.
  t = deadline_ns(1 * 1000 * 1000);
  TEST(pthread_rwlock_timedwrlock(&rw, &t) == ETIMEDOUT);
  return NULL;
}

static void *wr_acquires(void *arg) {
  (void)arg;
  struct timespec t = deadline_ns(10ll * 1000000000);
  TEST(pthread_rwlock_timedwrlock(&rw, &t) == 0);
  TEST(pthread_rwlock_unlock(&rw) == 0);
  return NULL;
}

static void *rd_acquires(void *arg) {
  (void)arg;
  struct timespec t = deadline_ns(10ll * 1000000000);
  TEST(pthread_rwlock_timedrdlock(&rw, &t) == 0);
  TEST(pthread_rwlock_unlock(&rw) == 0);
  return NULL;
}

int main(void) {
  pthread_t t;

  // Timeouts while a writer holds the lock.
  TEST(pthread_rwlock_wrlock(&rw) == 0);
  TEST(pthread_create(&t, NULL, against_writer, NULL) == 0);
  TEST(pthread_join(t, NULL) == 0);
  TEST(pthread_rwlock_unlock(&rw) == 0);

  // Behavior while a reader holds the lock.
  TEST(pthread_rwlock_rdlock(&rw) == 0);
  TEST(pthread_create(&t, NULL, against_reader, NULL) == 0);
  TEST(pthread_join(t, NULL) == 0);
  TEST(pthread_rwlock_unlock(&rw) == 0);

  // A timed writer is woken when the writer unlocks.
  TEST(pthread_rwlock_wrlock(&rw) == 0);
  TEST(pthread_create(&t, NULL, wr_acquires, NULL) == 0);
  for (int i = 0; i < 5; i++)
    TEST(sched_yield() == 0);
  TEST(pthread_rwlock_unlock(&rw) == 0);
  TEST(pthread_join(t, NULL) == 0);

  // A timed reader is woken when the writer unlocks.
  TEST(pthread_rwlock_wrlock(&rw) == 0);
  TEST(pthread_create(&t, NULL, rd_acquires, NULL) == 0);
  for (int i = 0; i < 5; i++)
    TEST(sched_yield() == 0);
  TEST(pthread_rwlock_unlock(&rw) == 0);
  TEST(pthread_join(t, NULL) == 0);

  return t_status;
}
