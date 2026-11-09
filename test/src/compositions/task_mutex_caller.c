#include "test.h"
#include <caller.h>

#define TEST(c)                                                                \
  do {                                                                         \
    if (!(c))                                                                  \
      t_error("%s failed\n", #c);                                              \
  } while (0)

static void wait_for(caller_subtask_t subtask) {
  caller_waitable_set_t set = caller_waitable_set_new();
  caller_waitable_join(subtask, set);

  caller_event_t event;
  caller_waitable_set_wait(set, &event);
  TEST(event.event == CALLER_EVENT_SUBTASK);
  TEST(event.code == CALLER_SUBTASK_RETURNED);
  caller_waitable_join(subtask, 0);
  caller_subtask_drop(subtask);
  caller_waitable_set_drop(set);
}

int main(void) {
  // First up, start a task that'll spawn a thread later. Then have another
  // task lock a mutex and exit, freeing its thread index for the thread that's
  // spawned when the first task resumes.
  caller_subtask_status_t status = test_simple_i_spawn_later();
  TEST(CALLER_SUBTASK_STATE(status) == CALLER_SUBTASK_STARTED);
  caller_subtask_t subtask = CALLER_SUBTASK_HANDLE(status);
  test_simple_i_lock();
  test_simple_i_release_spawn();
  wait_for(subtask);

  // Next have one task hold a number of locks while another task, the same
  // libc thread, operates on them.
  status = test_simple_i_hold();
  TEST(CALLER_SUBTASK_STATE(status) == CALLER_SUBTASK_STARTED);
  subtask = CALLER_SUBTASK_HANDLE(status);
  test_simple_i_barge_in();
  test_simple_i_release();
  wait_for(subtask);

  return t_status + test_simple_i_t_status();
}
