#include <wasi/version.h>

#ifdef __wasip3__
#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <wasi/wasip3_block.h>

/// Waits for `waitable` and fills in `event.`
///
/// A `timeout` can optionally be specified as well. If the `timeout` is 0 then
/// this function will block indefinitely for the `waitable`.
static bool __wasilibc_waitable_block_on(uint32_t waitable,
                                         wasip3_event_t *event,
                                         monotonic_clock_duration_t timeout) {
  // If a timeout is requested then create a subtask using the monotonic-clock
  // interface which'll get added to the waitable-set below. If the timeout has
  // immediately elapsed then go ahead and poll the waitable set to see if the
  // waitable is ready.
  bool poll = false;
  wasip3_subtask_t timeout_subtask = 0;
  if (timeout != 0) {
    wasip3_subtask_status_t status = monotonic_clock_wait_for(timeout);
    if (WASIP3_SUBTASK_STATE(status) == WASIP3_SUBTASK_RETURNED) {
      poll = true;
    } else {
      timeout_subtask = WASIP3_SUBTASK_HANDLE(status);
    }
  }

  wasip3_waitable_set_t set = wasip3_waitable_set_new();
  wasip3_waitable_join(waitable, set);
  if (timeout_subtask)
    wasip3_waitable_join(timeout_subtask, set);
  if (poll) {
    wasip3_waitable_set_poll(set, event);
  } else {
    wasip3_waitable_set_wait(set, event);
  }

  // Clean up the temporary waitable-set.
  wasip3_waitable_join(waitable, 0);
  if (timeout_subtask)
    wasip3_waitable_join(timeout_subtask, 0);
  wasip3_waitable_set_drop(set);

  // If a timeout was present and the timeout is what fired, then return
  // `false` indicating that the requested event didn't happen. Otherwise
  // someting else happened and be sure to clean up the subtask anyway.
  if (timeout_subtask) {
    if (event->event == WASIP3_EVENT_SUBTASK &&
        event->waitable == timeout_subtask) {
      wasip3_subtask_drop(timeout_subtask);
      return false;
    }
    wasip3_subtask_cancel(timeout_subtask);
    wasip3_subtask_drop(timeout_subtask);
  }

  // If `poll` was used instead of wait, and if no event came out of poll, then
  // return `false` indicating there's nothing on `waitable`.
  if (poll && event->event == WASIP3_EVENT_NONE)
    return false;

  return true;
}

void __wasilibc_subtask_block_on_and_drop(wasip3_subtask_t handle) {
  wasip3_event_t event;
  bool ok = __wasilibc_waitable_block_on(handle, &event, 0);
  (void)ok;
  assert(ok);
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

int __wasilibc_future_block_on_timeout(wasip3_waitable_status_t status,
                                       uint32_t future,
                                       monotonic_clock_duration_t timeout,
                                       wasip3_cancel_t cancel) {
  // If the future's status for this operation is that it's blocked, then block
  // as necessary to resolve it.
  if (status == WASIP3_WAITABLE_STATUS_BLOCKED) {
    wasip3_event_t event;
    if (!__wasilibc_waitable_block_on(future, &event, timeout)) {
      // If the timeout happened then cancel this future, and if cancellation
      // actually goes through then return -1. If cancellation races with
      // completion fall through.
      status = cancel(future);
      if (WASIP3_WAITABLE_STATE(status) == WASIP3_WAITABLE_CANCELLED) {
        errno = ETIMEDOUT;
        return -1;
      }
    } else {
      // If the timeout didn't happen then the event must be for this future.
      assert(event.event == WASIP3_EVENT_FUTURE_READ ||
             event.event == WASIP3_EVENT_FUTURE_WRITE);
      assert(event.waitable == future);
      status = event.code;
    }
  }
  assert(WASIP3_WAITABLE_STATE(status) == WASIP3_WAITABLE_COMPLETED ||
         WASIP3_WAITABLE_STATE(status) == WASIP3_WAITABLE_DROPPED);
  assert(WASIP3_WAITABLE_COUNT(status) == 0);
  return 0;
}

ssize_t __wasilibc_stream_block_on_timeout(wasip3_waitable_status_t status,
                                           uint32_t stream, bool *closed,
                                           monotonic_clock_duration_t timeout,
                                           wasip3_cancel_t cancel) {
  if (status == WASIP3_WAITABLE_STATUS_BLOCKED) {
    wasip3_event_t event;
    if (!__wasilibc_waitable_block_on(stream, &event, timeout)) {
      // If cancellation goes through and no items raced with cancellation then
      // propagate the -1 for the failure status. Otherwise fall through to
      // return the number of items that actually got transferred which raced
      // with cancellation.
      status = cancel(stream);
      if (WASIP3_WAITABLE_STATE(status) == WASIP3_WAITABLE_CANCELLED &&
          WASIP3_WAITABLE_COUNT(status) == 0) {
        errno = ETIMEDOUT;
        return -1;
      }
    } else {
      assert(event.event == WASIP3_EVENT_STREAM_READ ||
             event.event == WASIP3_EVENT_STREAM_WRITE);
      assert(event.waitable == stream);
      status = event.code;
    }
  }
  assert(WASIP3_WAITABLE_STATE(status) == WASIP3_WAITABLE_COMPLETED ||
         WASIP3_WAITABLE_STATE(status) == WASIP3_WAITABLE_DROPPED ||
         WASIP3_WAITABLE_STATE(status) == WASIP3_WAITABLE_CANCELLED);
  *closed = WASIP3_WAITABLE_STATE(status) == WASIP3_WAITABLE_DROPPED;
  return WASIP3_WAITABLE_COUNT(status);
}

#endif
