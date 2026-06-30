#include "test.h"
#include <pthread.h>
#include <stdio.h>

#define TEST(c)                                                                \
  do {                                                                         \
    if (!(c))                                                                  \
      t_error("%s failed\n", #c);                                              \
  } while (0)

static void *locker(void *arg) {
  (void)arg;
  flockfile(stdout);
  return NULL;
}

int main(void) {
  pthread_t t;
  TEST(pthread_create(&t, NULL, locker, NULL) == 0);
  TEST(pthread_join(t, NULL) == 0);

  flockfile(stdout);
  funlockfile(stdout);

  return t_status;
}
