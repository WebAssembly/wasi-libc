#include "test.h"
#include <pthread.h>

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
  TEST(pthread_mutex_lock(&m) == 0);
  pthread_t t;
  // Runs eagerly and blocks on pthread_mutex_lock(&m).
  TEST(pthread_create(&t, NULL, producer, NULL) == 0);

  while (!ready) {
    TEST(pthread_cond_wait(&c, &m) == 0);
  }

  TEST(pthread_mutex_unlock(&m) == 0);
  TEST(pthread_join(t, NULL) == 0);
  TEST(ready == 1);
  return t_status;
}
