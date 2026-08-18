#include "test.h"
#include <callee.h>
#include <pthread.h>

#define TEST(c)                                                                \
  do {                                                                         \
    if (!(c))                                                                  \
      t_error("%s failed\n", #c);                                              \
  } while (0)

_Thread_local void *expected;
static pthread_key_t tls_key = 0;
static bool tls_key_initialized = false;
static int dtor_calls = 0;

static void dtor(void *ptr) {
  TEST(ptr == expected);
  dtor_calls += 1;
}

static pthread_key_t key() {
  if (!tls_key_initialized) {
    TEST(pthread_key_create(&tls_key, dtor) == 0);
    tls_key_initialized = true;
  }
  return tls_key;
}

callee_callback_code_t exports_test_simple_i_run_async(uint32_t a) {
  expected = (void *)a;
  TEST(pthread_setspecific(key(), expected) == 0);
  TEST(pthread_getspecific(key()) == expected);

  callee_thread_yield();

  TEST(pthread_getspecific(key()) == expected);

  return CALLEE_CALLBACK_CODE_YIELD;
}

callee_callback_code_t
exports_test_simple_i_run_async_callback(callee_event_t *event) {
  TEST(event->event == CALLEE_EVENT_NONE);
  TEST(pthread_getspecific(key()) == expected);

  exports_test_simple_i_run_async_return();

  return CALLEE_CALLBACK_CODE_EXIT;
}

void exports_test_simple_i_run_sync(uint32_t a) {
  expected = (void *)a;
  TEST(pthread_setspecific(key(), expected) == 0);
  TEST(pthread_getspecific(key()) == expected);
}

uint32_t exports_test_simple_i_num_dtors(void) { return dtor_calls; }

uint32_t exports_test_simple_i_t_status(void) { return t_status; }
