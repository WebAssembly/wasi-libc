#include "test.h"
#include <pthread.h>
#include <semaphore.h>
#include <stdatomic.h>

#define TEST(c)                                                                \
  do {                                                                         \
    if (!(c))                                                                  \
      t_error("%s failed\n", #c);                                              \
  } while (0)

static sem_t sem;
static atomic_int woke = 0;

static void *waiter(void *arg) {
  (void)arg;
  TEST(sem_wait(&sem) == 0);
  atomic_fetch_add(&woke, 1);
  return NULL;
}

int main(void) {
  TEST(sem_init(&sem, 0, 0) == 0);
  pthread_t a, b;
  TEST(pthread_create(&a, NULL, waiter, NULL) == 0);
  TEST(pthread_create(&b, NULL, waiter, NULL) == 0);

  TEST(sem_post(&sem) == 0);
  TEST(sem_post(&sem) == 0);

  TEST(pthread_join(a, NULL) == 0);
  TEST(pthread_join(b, NULL) == 0);

  TEST(atomic_load(&woke) == 2);
  return t_status;
}
