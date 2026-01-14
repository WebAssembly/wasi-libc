#include <wasi/wasip3_block.h>
#include <assert.h>

int wasip3_subtask_block_on(wasip3_subtask_status_t status) {
  if (status == WASIP3_SUBTASK_RETURNED) {
    return 1;
  } else if (WASIP3_SUBTASK_STATE(status) == WASIP3_SUBTASK_STARTING ||
             WASIP3_SUBTASK_STATE(status) == WASIP3_SUBTASK_STARTED) {
    wasip3_subtask_t handle = WASIP3_SUBTASK_HANDLE(status);
    wasip3_waitable_set_t set = wasip3_waitable_set_new();
    wasip3_waitable_join(handle, set);
    wasip3_event_t event;
    wasip3_waitable_set_wait(set, &event);
    assert(event.event == WASIP3_EVENT_SUBTASK);
    wasip3_subtask_drop(event.waitable);
    wasip3_waitable_set_drop(set);
    if (event.code == WASIP3_SUBTASK_RETURNED) {
      return 1;
    } else if (event.code == WASIP3_SUBTASK_STARTING) {
      return 0;
    } else {
      // fall through to cancelled
    }
  }
  // WASIP3_SUBTASK_STARTED_CANCELLED
  // WASIP3_SUBTASK_RETURNED_CANCELLED
  return -1;
}
