#include "test.h"
#include <pthread.h>

#define TEST(c)                                                                \
  do {                                                                         \
    if (!(c))                                                                  \
      t_error("%s failed\n", #c);                                              \
  } while (0)

static void *worker(void *arg) {
  int *p = arg;
  *p = 42;
  return (void *)0x1234;
}

int main(void) {
  pthread_t t;
  int value = 0;
  TEST(pthread_create(&t, NULL, worker, &value) == 0);
  void *ret = NULL;
  TEST(pthread_join(t, &ret) == 0);
  TEST(value == 42);
  TEST(ret == (void *)0x1234);
  return t_status;
}
