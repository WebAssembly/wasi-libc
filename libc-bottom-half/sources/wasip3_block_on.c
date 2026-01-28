#include <wasi/version.h>

#ifdef __wasip3__
#include <assert.h>
#include <stdlib.h>
#include <wasi/wasip3_block.h>

void wasip3_subtask_block_on(wasip3_subtask_status_t status) {
  // we don't encounter cancelled state because this function won't cancel
  if (WASIP3_SUBTASK_STATE(status) == WASIP3_SUBTASK_STARTING ||
      WASIP3_SUBTASK_STATE(status) == WASIP3_SUBTASK_STARTED) {
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

ssize_t wasip3_waitable_block_on(wasip3_waitable_status_t status,
                                 waitable_t stream) {
  if (status == WASIP3_WAITABLE_STATUS_BLOCKED) {
    wasip3_waitable_set_t set = wasip3_waitable_set_new();
    wasip3_waitable_join(stream, set);
    wasip3_event_t event;
    wasip3_waitable_set_wait(set, &event);
    assert(event.event == WASIP3_EVENT_STREAM_WRITE ||
           event.event == WASIP3_EVENT_STREAM_READ ||
           event.event == WASIP3_EVENT_FUTURE_READ);
    assert(event.waitable == stream);
    // remove from set
    wasip3_waitable_join(stream, 0);
    wasip3_waitable_set_drop(set);
    ssize_t amount = event.event == WASIP3_EVENT_FUTURE_READ ? 1 : event.code;
    return amount;
  } else if (WASIP3_WAITABLE_STATE(status) == WASIP3_WAITABLE_COMPLETED ||
             WASIP3_WAITABLE_STATE(status) == WASIP3_WAITABLE_DROPPED) {
    ssize_t amount = WASIP3_WAITABLE_COUNT(status);
    return amount;
  } else {
    abort();
  }
}

#endif
