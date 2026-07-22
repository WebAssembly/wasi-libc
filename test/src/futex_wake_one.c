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
static volatile int woke_count;
static int waiter_result[NUM_WAITERS];

static void *waiter(void *arg) {
  int id = (int)(intptr_t)arg;

  // Mark this waiter as ready, then wait until all waiters are released.
  __atomic_fetch_add(&ready_count, 1, __ATOMIC_SEQ_CST);
  while (!__atomic_load_n(&go_flag, __ATOMIC_SEQ_CST)) {
    usleep(1000);
  }

  __atomic_fetch_add(&waiting_count, 1, __ATOMIC_SEQ_CST);
  waiter_result[id] = __wasilibc_futex_wait((volatile void *)&futex_word, 0,
                                            CLOCK_REALTIME, NULL, 0);
  if (waiter_result[id] == 0) {
    __atomic_fetch_add(&woke_count, 1, __ATOMIC_SEQ_CST);
  }
  return NULL;
}

int main(void) {
  pthread_t threads[NUM_WAITERS];

  futex_word = 0;
  ready_count = 0;
  go_flag = 0;
  waiting_count = 0;
  woke_count = 0;

  for (int i = 0; i < NUM_WAITERS; i++) {
    waiter_result[i] = -9999;
    TEST(pthread_create(&threads[i], NULL, waiter, (void *)(intptr_t)i) == 0);
  }

  while (__atomic_load_n(&ready_count, __ATOMIC_SEQ_CST) != NUM_WAITERS) {
    usleep(1000);
  }

  // Let all waiters proceed into the futex wait call.
  __atomic_store_n(&go_flag, 1, __ATOMIC_SEQ_CST);

  while (__atomic_load_n(&waiting_count, __ATOMIC_SEQ_CST) != NUM_WAITERS) {
    usleep(1000);
  }

  // Wake exactly one waiter first.
  TEST(__wasilibc_futex_wake((volatile int *)&futex_word, 1, 0) == 1);

  // Give the awakened thread a chance to run and record the wake.
  for (int i = 0;
       i < 1000 && __atomic_load_n(&woke_count, __ATOMIC_SEQ_CST) == 0; i++) {
    usleep(1000);
  }

  TEST(__atomic_load_n(&woke_count, __ATOMIC_SEQ_CST) == 1);

  // Then wake the rest and ensure everyone completed successfully.
  TEST(__wasilibc_futex_wake((volatile int *)&futex_word,
                             __WASILIBC_FUTEX_WAKE_ALL, 0) == NUM_WAITERS - 1);

  for (int i = 0; i < NUM_WAITERS; i++) {
    TEST(pthread_join(threads[i], NULL) == 0);
    TEST(waiter_result[i] == 0);
  }

  return t_status;
}
