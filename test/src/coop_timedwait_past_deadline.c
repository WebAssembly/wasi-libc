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
static sem_t sem;

static void *contender(void *arg) {
  (void)arg;
  struct timespec deadline = {.tv_sec = -1, .tv_nsec = 0};
  TEST(pthread_mutex_timedlock(&lock, &deadline) == ETIMEDOUT);

  errno = 0;
  TEST(sem_timedwait(&sem, &deadline) == -1);
  TEST(errno == ETIMEDOUT);
  return NULL;
}

int main(void) {
  TEST(sem_init(&sem, 0, 0) == 0);
  pthread_t t;
  TEST(pthread_mutex_lock(&lock) == 0);
  TEST(pthread_create(&t, NULL, contender, NULL) == 0);
  TEST(pthread_join(t, NULL) == 0);
  TEST(pthread_mutex_unlock(&lock) == 0);
  return t_status;
}
