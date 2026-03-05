#include <wasi/version.h>

#ifdef __wasip3__
#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <wasi/wasip3_block.h>

static void waitable_block_on(uint32_t waitable, wasip3_event_t *event) {
  wasip3_waitable_set_t set = wasip3_waitable_set_new();
  wasip3_waitable_join(waitable, set);
  wasip3_waitable_set_wait(set, event);
  // remove from set
  wasip3_waitable_join(waitable, 0);
  wasip3_waitable_set_drop(set);
}

void __wasilibc_subtask_block_on_and_drop(wasip3_subtask_t handle) {
  wasip3_event_t event;
  waitable_block_on(handle, &event);
  assert(event.event == WASIP3_EVENT_SUBTASK);
  assert(event.waitable == handle);
  assert(event.code == WASIP3_SUBTASK_RETURNED);
  wasip3_subtask_drop(handle);
}

int __wasilibc_subtask_await_nonblocking(wasip3_subtask_status_t status) {
  if (WASIP3_SUBTASK_STATE(status) == WASIP3_SUBTASK_RETURNED)
    return 0;
  wasip3_subtask_t subtask = WASIP3_SUBTASK_HANDLE(status);
  status = wasip3_subtask_cancel(subtask);
  wasip3_subtask_drop(subtask);
  if (WASIP3_SUBTASK_STATE(status) != WASIP3_SUBTASK_RETURNED) {
    assert(WASIP3_SUBTASK_STATE(status) == WASIP3_SUBTASK_STARTED_CANCELLED ||
           WASIP3_SUBTASK_STATE(status) == WASIP3_SUBTASK_RETURNED_CANCELLED);
    errno = EWOULDBLOCK;
    return -1;
  }
  return 0;
}

void __wasilibc_future_block_on(wasip3_waitable_status_t status,
                                uint32_t future) {
  if (status == WASIP3_WAITABLE_STATUS_BLOCKED) {
    wasip3_event_t event;
    waitable_block_on(future, &event);
    assert(event.event == WASIP3_EVENT_FUTURE_READ ||
           event.event == WASIP3_EVENT_FUTURE_WRITE);
    assert(event.waitable == future);
    status = event.code;
  }
  assert(WASIP3_WAITABLE_STATE(status) == WASIP3_WAITABLE_COMPLETED ||
         WASIP3_WAITABLE_STATE(status) == WASIP3_WAITABLE_DROPPED);
  assert(WASIP3_WAITABLE_COUNT(status) == 0);
}

size_t __wasilibc_stream_block_on(wasip3_waitable_status_t status,
                                  uint32_t stream, bool *closed) {
  if (status == WASIP3_WAITABLE_STATUS_BLOCKED) {
    wasip3_event_t event;
    waitable_block_on(stream, &event);
    assert(event.event == WASIP3_EVENT_STREAM_READ ||
           event.event == WASIP3_EVENT_STREAM_WRITE);
    assert(event.waitable == stream);
    status = event.code;
  }
  assert(WASIP3_WAITABLE_STATE(status) == WASIP3_WAITABLE_COMPLETED ||
         WASIP3_WAITABLE_STATE(status) == WASIP3_WAITABLE_DROPPED);
  *closed = WASIP3_WAITABLE_STATE(status) == WASIP3_WAITABLE_DROPPED;
  return WASIP3_WAITABLE_COUNT(status);
}

#endif
