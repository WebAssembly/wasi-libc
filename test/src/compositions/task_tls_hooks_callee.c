#include "test.h"
#include <callee.h>
#include <stdio.h>

#define TEST(c)                                                                \
  do {                                                                         \
    if (!(c))                                                                  \
      t_error("%s failed\n", #c);                                              \
  } while (0)

_Thread_local int current_value = 0;

callee_callback_code_t exports_test_simple_i_count_up() {
  fprintf(stderr, "howdy0 %p=%d\n", &current_value, current_value);
  current_value++;
  fprintf(stderr, "howdy1 %p=%d\n", &current_value, current_value);

  for (int i = 0; i < 2; i++) {
    callee_thread_yield();
    current_value++;
    fprintf(stderr, "howdy2 %p=%d\n", &current_value, current_value);
  }

  return CALLEE_CALLBACK_CODE_YIELD;
}

callee_callback_code_t
exports_test_simple_i_count_up_callback(callee_event_t *event) {
  TEST(event->event == CALLEE_EVENT_NONE);

  current_value++;
  fprintf(stderr, "howdy3 %p=%d\n", &current_value, current_value);

  exports_test_simple_i_count_up_return();

  return CALLEE_CALLBACK_CODE_EXIT;
}

void exports_test_simple_i_barge_in() {
  current_value++;
}

uint32_t exports_test_simple_i_t_status(void) { return t_status; }

uint32_t exports_test_simple_i_total(void) { return current_value; }
