#include "test.h"
#include <errno.h>
#include <pthread.h>
#include <sched.h>
#include <stdint.h>
#include <time.h>
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
static volatile int ready_count;
static volatile int go_flag;
static volatile int waiting_count;
static int waiter_result[NUM_WAITERS];

static void *waiter(void *arg) {
  int id = (int)(intptr_t)arg;

  // Mark this waiter as ready, then wait for a synchronized start.
  __atomic_fetch_add(&ready_count, 1, __ATOMIC_SEQ_CST);
  while (!__atomic_load_n(&go_flag, __ATOMIC_SEQ_CST)) {
    usleep(1000);
  }

  __atomic_fetch_add(&waiting_count, 1, __ATOMIC_SEQ_CST);
  waiter_result[id] = __wasilibc_futex_wait((volatile void *)&futex_word, 0,
                                            CLOCK_REALTIME, NULL, 0);
  return NULL;
}

int main(void) {
  pthread_t threads[NUM_WAITERS];

  futex_word = 0;
  ready_count = 0;
  go_flag = 0;
  waiting_count = 0;

  for (int i = 0; i < NUM_WAITERS; i++) {
    waiter_result[i] = -9999;
    TEST(pthread_create(&threads[i], NULL, waiter, (void *)(intptr_t)i) == 0);
  }

  while (__atomic_load_n(&ready_count, __ATOMIC_SEQ_CST) != NUM_WAITERS) {
    usleep(1000);
  }

  // Let all waiters enter the futex wait path before issuing wake-all.
  __atomic_store_n(&go_flag, 1, __ATOMIC_SEQ_CST);

  while (__atomic_load_n(&waiting_count, __ATOMIC_SEQ_CST) != NUM_WAITERS) {
    usleep(1000);
  }

  // Wake all waiters and expect each wait call to succeed.
  TEST(__wasilibc_futex_wake((volatile int *)&futex_word,
                             __WASILIBC_FUTEX_WAKE_ALL, 0) == NUM_WAITERS);

  for (int i = 0; i < NUM_WAITERS; i++) {
    TEST(pthread_join(threads[i], NULL) == 0);
    TEST(waiter_result[i] == 0);
  }

  return t_status;
}
