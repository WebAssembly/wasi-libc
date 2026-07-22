#include "test.h"
#include <errno.h>
#include <pthread.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>
#include <wasi/libc.h>

#define TEST(c)                                                                \
  do {                                                                         \
    if (!(c))                                                                  \
      t_error("%s failed\n", #c);                                              \
  } while (0)

enum {
  NUM_WAITERS = 3,
};

static volatile int futex_word;
static volatile int waiting_count;
static volatile int done_flag;
static volatile int wake_count[NUM_WAITERS];

static int total_wakes(void) {
  int total = 0;
  for (int i = 0; i < NUM_WAITERS; i++)
    total += __atomic_load_n(&wake_count[i], __ATOMIC_SEQ_CST);
  return total;
}

static void *waiter(void *arg) {
  int id = (int)(intptr_t)arg;

  while (!__atomic_load_n(&done_flag, __ATOMIC_SEQ_CST)) {
    __atomic_fetch_add(&waiting_count, 1, __ATOMIC_SEQ_CST);
    int rc = __wasilibc_futex_wait((volatile void *)&futex_word, 0,
                                   CLOCK_REALTIME, NULL, 0);
    __atomic_fetch_sub(&waiting_count, 1, __ATOMIC_SEQ_CST);
    if (rc != 0) {
      t_error("waiter %d: futex_wait returned %d\n", id, rc);
      return NULL;
    }
    __atomic_fetch_add(&wake_count[id], 1, __ATOMIC_SEQ_CST);
  }
  return NULL;
}

int main(void) {
  pthread_t threads[NUM_WAITERS];

  for (int i = 0; i < NUM_WAITERS; i++)
    TEST(pthread_create(&threads[i], NULL, waiter, (void *)(intptr_t)i) == 0);

  while (__atomic_load_n(&waiting_count, __ATOMIC_SEQ_CST) != NUM_WAITERS)
    TEST(sched_yield() == 0);

  TEST(__wasilibc_futex_wake((volatile int *)&futex_word, NUM_WAITERS,
                             __WASILIBC_FUTEX_YIELD) == NUM_WAITERS);

  while (total_wakes() != NUM_WAITERS)
    TEST(sched_yield() == 0);

  for (int i = 0; i < NUM_WAITERS; i++) {
    int woken = __atomic_load_n(&wake_count[i], __ATOMIC_SEQ_CST);
    if (woken != 1)
      t_error("waiter %d woke %d times, expected 1\n", i, woken);
  }

  __atomic_store_n(&done_flag, 1, __ATOMIC_SEQ_CST);
  while (__atomic_load_n(&waiting_count, __ATOMIC_SEQ_CST) != 0) {
    __wasilibc_futex_wake((volatile int *)&futex_word,
                          __WASILIBC_FUTEX_WAKE_ALL, 0);
    TEST(sched_yield() == 0);
  }

  for (int i = 0; i < NUM_WAITERS; i++)
    TEST(pthread_join(threads[i], NULL) == 0);

  return t_status;
}
