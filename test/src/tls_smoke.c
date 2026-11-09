#include "test.h"
#include <pthread.h>
#if defined(__wasip1__) && defined(_REENTRANT)
#include <wasi/libc-busywait.h>
#endif

#define TEST(c)                                                                \
  do {                                                                         \
    if (!(c))                                                                  \
      t_error("%s failed\n", #c);                                              \
  } while (0)

static __thread int static_tls = 3;

static void *worker(void *arg) {
  (void)arg;

  TEST(static_tls == 3);
  static_tls = 5;

  return NULL;
}

int main(void) {
#if defined(__wasip1__) && defined(_REENTRANT)
  // `pthread_join` below may need to block, and the main thread of a browser
  // can't execute `memory.atomic.wait32`, so spin instead.
  __wasilibc_enable_futex_busywait_on_current_thread();
#endif

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
