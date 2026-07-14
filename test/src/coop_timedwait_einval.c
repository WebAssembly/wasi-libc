#include "test.h"
#include <errno.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

#define TEST(c)                                                                \
  do {                                                                         \
    if (!(c))                                                                  \
      t_error("%s failed\n", #c);                                              \
  } while (0)

static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
static pthread_rwlock_t rw = PTHREAD_RWLOCK_INITIALIZER;
static sem_t sem;

static void *einval_checks(void *arg) {
  (void)arg;
  struct timespec too_big = {.tv_sec = 0, .tv_nsec = 1000000000};
  struct timespec negative = {.tv_sec = 0, .tv_nsec = -1};

  // All of these are contended (main holds `lock` and a write lock on `rw`,
  // and `sem` has no permits), so blocking would occur and the invalid
  // timespec must be reported.
  TEST(pthread_mutex_timedlock(&lock, &too_big) == EINVAL);
  TEST(pthread_mutex_timedlock(&lock, &negative) == EINVAL);
  TEST(pthread_rwlock_timedrdlock(&rw, &too_big) == EINVAL);
  TEST(pthread_rwlock_timedwrlock(&rw, &too_big) == EINVAL);

  errno = 0;
  TEST(sem_timedwait(&sem, &too_big) == -1);
  TEST(errno == EINVAL);

  // pthread_cond_timedwait must return EINVAL with the mutex still held: the
  // errorcheck mutex successfully unlocks afterwards, which it wouldn't if
  // the wait had released it (or left it in a bad state).
  pthread_mutex_t m2;
  pthread_mutexattr_t ma;
  pthread_cond_t c2 = PTHREAD_COND_INITIALIZER;
  TEST(pthread_mutexattr_init(&ma) == 0);
  TEST(pthread_mutexattr_settype(&ma, PTHREAD_MUTEX_ERRORCHECK) == 0);
  TEST(pthread_mutex_init(&m2, &ma) == 0);
  TEST(pthread_mutex_lock(&m2) == 0);
  TEST(pthread_cond_timedwait(&c2, &m2, &too_big) == EINVAL);
  TEST(pthread_cond_timedwait(&c2, &m2, &negative) == EINVAL);
  TEST(pthread_mutex_unlock(&m2) == 0);
  return NULL;
}

int main(void) {
  TEST(sem_init(&sem, 0, 0) == 0);
  TEST(pthread_mutex_lock(&lock) == 0);
  TEST(pthread_rwlock_wrlock(&rw) == 0);

  pthread_t t;
  TEST(pthread_create(&t, NULL, einval_checks, NULL) == 0);
  TEST(pthread_join(t, NULL) == 0);

  TEST(pthread_rwlock_unlock(&rw) == 0);
  TEST(pthread_mutex_unlock(&lock) == 0);
  return t_status;
}
