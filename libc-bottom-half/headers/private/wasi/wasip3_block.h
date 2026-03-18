#ifndef WASI_WASIP3_BLOCK_H
#define WASI_WASIP3_BLOCK_H

#include <wasi/api.h>

#ifdef __wasip3__
#include <stdint.h>
#include <wasi/descriptor_table.h> // for waitable_t

typedef wasip3_waitable_status_t (*wasip3_cancel_t)(uint32_t);

// Waits for a subtask to return
void __wasilibc_subtask_block_on_and_drop(wasip3_subtask_t subtask);

// Blocks the current thread until the subtask invocation indicated by `status`
// has completed.
//
// This will block the current thread until the `status`'s subtask, if present,
// has completed. This function takes ownership of the subtask indicated by
// `subtask`.
static inline void __wasilibc_subtask_await(wasip3_subtask_status_t status) {
  if (WASIP3_SUBTASK_STATE(status) != WASIP3_SUBTASK_RETURNED)
    __wasilibc_subtask_block_on_and_drop(WASIP3_SUBTASK_HANDLE(status));
}

// Attempts to see if `status` indicates a completed task. If it does not then
// the task is cancelled.
//
// This method is unlike `__wasilibc_subtask_await` in that it will not block
// the current thread.
//
// This function returns 0 if the subtask completed, and otherwise returns -1
// and sets `errno` to `EWOULDBLOCK` if the subtask was successfully cancelled.
int __wasilibc_subtask_await_nonblocking(wasip3_subtask_status_t status);

/// Waits for `future` to be resolved after a previous operation yielded
/// `status` as a result.
///
/// This function will block if `status` indicates that the future is blocked,
/// and this function won't return until the future has become ready again. This
/// is suitable, for example, for waiting for future reads to complete.
///
/// If `timeout` is nonzero then this function will block for at most that
/// amount of time. The `cancel` callback must be set when `timeout` is set and
/// that will be used to cancel the in-flight operation on this future.
int __wasilibc_future_block_on_timeout(wasip3_waitable_status_t status,
                                       uint32_t future,
                                       monotonic_clock_duration_t timeout,
                                       wasip3_cancel_t cancel);

// Convenience wrapper around `*_timeout` above which doesn't take a timeout.
static inline void __wasilibc_future_block_on(wasip3_waitable_status_t status,
                                              uint32_t future) {
  ssize_t ret = __wasilibc_future_block_on_timeout(status, future, 0, NULL);
  (void)ret;
  assert(ret >= 0);
}

/// Waits for `stream` to be resolved after a previous operation yielded
/// `status` as a result.
///
/// This function will block if `status` indicates that the stream is blocked,
/// and this function won't return until the stream has become ready again. This
/// is suitable, for example, for waiting for stream read or write to complete.
///
/// The `closed` variable is set based on the result of the operation to
/// communicate what was received from the component model.
///
/// If `timeout` is nonzero then this function will block for at most that
/// amount of time. The `cancel` callback must be set when `timeout` is set and
/// that will be used to cancel the in-flight operation on this stream.
ssize_t __wasilibc_stream_block_on_timeout(wasip3_waitable_status_t status,
                                           uint32_t stream, bool *closed,
                                           monotonic_clock_duration_t timeout,
                                           wasip3_cancel_t cancel);

// Convenience wrapper around `*_timeout` above which doesn't take a timeout.
static inline size_t __wasilibc_stream_block_on(wasip3_waitable_status_t status,
                                                uint32_t stream, bool *closed) {
  ssize_t ret =
      __wasilibc_stream_block_on_timeout(status, stream, closed, 0, NULL);
  assert(ret >= 0);
  return ret;
}

/// TODO
void __wasilibc_poll_waitable(uint32_t waitable, wasip3_event_t *event);

#endif // __wasip3__

#endif // WASI_WASIP3_BLOCK_H
