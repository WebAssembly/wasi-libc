#include "test.h"
#include <pthread.h>
#include <stdio.h>

#define TEST(c)                                                                \
  do {                                                                         \
    if (!(c))                                                                  \
      t_error("%s failed\n", #c);                                              \
  } while (0)

static void *worker(void *arg) {
  (void)arg;
  return NULL;
}

int main(void) {
  // Pull in extra management of locking w.r.t. threads.
  flockfile(stdout);
  funlockfile(stdout);

  pthread_key_t key;
  TEST(pthread_key_create(&key, NULL) == 0);

  pthread_t t;
  TEST(pthread_create(&t, NULL, worker, NULL) == 0);
  TEST(pthread_join(t, NULL) == 0);

  TEST(pthread_key_delete(key) == 0);

  return t_status;
}
