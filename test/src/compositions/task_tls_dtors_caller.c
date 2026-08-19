#include "test.h"
#include <caller.h>
#include <stdio.h>

#define TEST(c)                                                                \
  do {                                                                         \
    if (!(c))                                                                  \
      t_error("%s failed\n", #c);                                              \
  } while (0)

#define N 10

int main(void) {
  caller_subtask_t tasks[N];

  for (int i = 0; i < N; i++) {
    caller_subtask_status_t status = test_simple_i_run_async(i + N);
    TEST(CALLER_SUBTASK_STATE(status) == CALLER_SUBTASK_STARTED ||
         CALLER_SUBTASK_STATE(status) == CALLER_SUBTASK_STARTING);
    tasks[i] = CALLER_SUBTASK_HANDLE(status);

    test_simple_i_run_sync(i + 2 * N);
  }

  caller_waitable_set_t set = caller_waitable_set_new();
  for (int i = 0; i < N; i++)
    caller_waitable_join(tasks[i], set);

  int ntasks = N;
  while (ntasks) {
    caller_event_t event;
    caller_waitable_set_wait(set, &event);
    TEST(event.event == CALLER_EVENT_SUBTASK);
    if (event.code == CALLER_SUBTASK_RETURNED) {
      ntasks -= 1;
      caller_subtask_drop(event.waitable);
    }
  }

  fprintf(stderr, "dtor_calls = %d\n", test_simple_i_num_dtors());
  TEST(test_simple_i_num_dtors() == 2 * N);

  return t_status + test_simple_i_t_status();
}
