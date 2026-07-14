#include "test.h"
#include <errno.h>
#include <pthread.h>
#include <time.h>

#define TEST(c)                                                                \
  do {                                                                         \
    if (!(c))                                                                  \
      t_error("%s failed\n", #c);                                              \
  } while (0)

static pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t c = PTHREAD_COND_INITIALIZER;
static int ready = 0;

static void *producer(void *arg) {
  (void)arg;
  TEST(pthread_mutex_lock(&m) == 0);
  ready = 1;
  TEST(pthread_cond_signal(&c) == 0);
  TEST(pthread_mutex_unlock(&m) == 0);
  return NULL;
}

int main(void) {
  // actually time out
  {
    struct timespec ts = {.tv_sec = 0, .tv_nsec = 100 * 1000 * 1000};
    TEST(pthread_mutex_lock(&m) == 0);
    int rc = pthread_cond_timedwait(&c, &m, &ts);
    TEST(rc == ETIMEDOUT);
    TEST(pthread_mutex_unlock(&m) == 0);
  }

  // don't time out
  {
    struct timespec ts;
    TEST(clock_gettime(CLOCK_REALTIME, &ts) == 0);
    ts.tv_sec += 30;

    TEST(pthread_mutex_lock(&m) == 0);
    pthread_t t;
    TEST(pthread_create(&t, NULL, producer, NULL) == 0);

    TEST(!ready);
    TEST(pthread_cond_timedwait(&c, &m, &ts) == 0);
    TEST(ready);

    TEST(pthread_mutex_unlock(&m) == 0);
    TEST(pthread_join(t, NULL) == 0);
  }

  return t_status;
}
