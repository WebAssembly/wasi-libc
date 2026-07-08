#include "test.h"
#include <pthread.h>

#define TEST(c)                                                                \
  do {                                                                         \
    if (!(c))                                                                  \
      t_error("%s failed\n", #c);                                              \
  } while (0)

static __thread int static_tls = 3;

static void *worker(void *arg) {
  (void) arg;

  TEST(static_tls == 3);
  static_tls = 5;

  return NULL;
}

int main(void) {
  TEST(static_tls == 3);
  static_tls = 4;
  pthread_t t;
  if (pthread_create(&t, NULL, worker, NULL) == 0) {
    TEST(static_tls == 4);
    TEST(pthread_join(t, NULL) == 0);
  }
  TEST(static_tls == 4);

  return t_status;
}
