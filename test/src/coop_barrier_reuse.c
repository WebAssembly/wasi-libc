#include "test.h"
#include <pthread.h>
#include <stdatomic.h>

#define TEST(c)                                                                \
  do {                                                                         \
    if (!(c))                                                                  \
      t_error("%s failed\n", #c);                                              \
  } while (0)

#define ROUNDS 3

static pthread_barrier_t barrier;
static atomic_int total = 0;
static atomic_int serial = 0;

static void participate(void) {
  for (int i = 0; i < ROUNDS; i++) {
    int r = pthread_barrier_wait(&barrier);
    atomic_fetch_add(&total, 1);
    if (r == PTHREAD_BARRIER_SERIAL_THREAD)
      atomic_fetch_add(&serial, 1);
    else
      TEST(r == 0);
  }
}

static void *worker(void *arg) {
  (void)arg;
  participate();
  return NULL;
}

int main(void) {
  TEST(pthread_barrier_init(&barrier, NULL, 2) == 0);

  pthread_t t;
  TEST(pthread_create(&t, NULL, worker, NULL) == 0);
  participate();

  TEST(pthread_join(t, NULL) == 0);
  TEST(pthread_barrier_destroy(&barrier) == 0);

  TEST(total == 2 * ROUNDS);
  TEST(serial == ROUNDS);
  return t_status;
}
