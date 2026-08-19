#include "test.h"
#include <callee.h>

#define N 200
#define TEST(c)                                                                \
  do {                                                                         \
    if (!(c))                                                                  \
      t_error("%s failed\n", #c);                                              \
  } while (0)

callee_callback_code_t exports_test_simple_i_blocking(void) {
  int my_stack_array[N];

  for (int i = 0; i < N; i++)
    my_stack_array[i] = i;

  callee_thread_yield();

  for (int i = 0; i < N; i++)
    TEST(my_stack_array[i] == i);

  exports_test_simple_i_blocking_return();
  return CALLEE_CALLBACK_CODE_EXIT;
}

callee_callback_code_t
exports_test_simple_i_blocking_callback(callee_event_t *event) {
  (void)event;
  __builtin_trap();
}

void exports_test_simple_i_barge_in() {
  int my_stack_array[N];

  for (int i = 0; i < N; i++)
    my_stack_array[i] = i + N;

  for (int i = 0; i < N; i++)
    TEST(my_stack_array[i] == i + N);
}

void exports_test_simple_i_barge_in_with_post_return(callee_string_t *ret) {
  ret->ptr = NULL;
  ret->len = 0;

  // smash the stack a bit here too.
  exports_test_simple_i_barge_in();
}

uint32_t exports_test_simple_i_t_status() { return t_status; }
