#include <wasi/version.h>

#ifdef __wasip3__
#include <assert.h>
#include <wasi/wasip3_block.h>

void wasip3_subtask_block_on(wasip3_subtask_status_t status) {
  // we don't encounter cancelled state because this function won't cancel
  if (WASIP3_SUBTASK_STATE(status) != WASIP3_SUBTASK_RETURNED) {
    wasip3_subtask_t handle = WASIP3_SUBTASK_HANDLE(status);
    wasip3_waitable_set_t set = wasip3_waitable_set_new();
    wasip3_waitable_join(handle, set);
    wasip3_event_t event;
    event.code = WASIP3_SUBTASK_STATE(status);
    while (event.code != WASIP3_SUBTASK_RETURNED) {
      wasip3_waitable_set_wait(set, &event);
      assert(event.event == WASIP3_EVENT_SUBTASK);
      assert(event.waitable == handle);
    }
    wasip3_subtask_drop(event.waitable);
    wasip3_waitable_set_drop(set);
  }
}
#endif
