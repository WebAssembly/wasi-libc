#include "test.h"
#include <pthread.h>
#include <sched.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <wasi/libc.h>

#define TEST(c)                                                                \
  do {                                                                         \
    if (!(c))                                                                  \
      t_error("%s failed\n", #c);                                              \
  } while (0)

#define NUM_WAITERS 3

static volatile int futex_word;
static volatile int waiting_count;
static volatile int woken_count;

static void **malloc_traffic;

static void *waiter(void *arg) {
  (void)arg;

  __atomic_fetch_add(&waiting_count, 1, __ATOMIC_SEQ_CST);
  TEST(__wasilibc_futex_wait((volatile void *)&futex_word, 0, CLOCK_REALTIME,
                             NULL, 0) == 0);

  if (__atomic_fetch_add(&woken_count, 1, __ATOMIC_SEQ_CST) == 0) {
    malloc_traffic = malloc(sizeof(void *));
    TEST(malloc_traffic != NULL);
    *malloc_traffic = NULL;
  }

  TEST(sched_yield() == 0);
  return NULL;
}

int main(void) {
  pthread_t threads[NUM_WAITERS];

  for (int i = 0; i < NUM_WAITERS; i++)
    TEST(pthread_create(&threads[i], NULL, waiter, NULL) == 0);

  while (__atomic_load_n(&waiting_count, __ATOMIC_SEQ_CST) != NUM_WAITERS)
    TEST(sched_yield() == 0);

  TEST(__wasilibc_futex_wake((volatile int *)&futex_word,
                             __WASILIBC_FUTEX_WAKE_ALL, 0) == NUM_WAITERS);

  for (int i = 0; i < NUM_WAITERS; i++)
    TEST(pthread_join(threads[i], NULL) == 0);

  // `malloc_traffic` is still live, so no other allocation may alias it.
  void **other = malloc(sizeof(void *));
  TEST(other != NULL);
  TEST(other != malloc_traffic);
  TEST(*malloc_traffic == NULL);

  return t_status;
}
