#include "common/time.h"
#include "pthread_impl.h"
#include <assert.h>
#include <stdatomic.h>
#include <time.h>
#include <wasi/api.h>

#ifndef __wasip3__
#error "Unknown WASI version"
#endif

// Manual bindings to a `future` in the component model (no payload) where the
// name mangling is what's recognized by `wit-component` at this time.
__attribute__((import_module("$root"),
               import_name("[future-new-unit]"))) extern uint64_t
future_new_unit(void);
__attribute__((import_module("$root"),
               import_name("[future-drop-readable-unit]"))) extern void
    future_drop_readable_unit(uint32_t);
__attribute__((import_module("$root"),
               import_name("[future-drop-writable-unit]"))) extern void
    future_drop_writable_unit(uint32_t);
__attribute__((import_module("$root"),
               import_name("[future-cancel-read-unit]"))) extern uint32_t
    future_cancel_read_unit(uint32_t);
__attribute__((import_module("$root"),
               import_name("[async-lower][future-write-unit]"))) extern uint32_t
future_write_unit(uint32_t future, const void *ptr);
__attribute__((import_module("$root"),
               import_name("[async-lower][future-read-unit]"))) extern uint32_t
future_read_unit(uint32_t future, void *ptr);

static void list_insert(struct __waitlist_node **list,
                        struct __waitlist_node *node) {
  node->next = *list;
  node->prev = NULL;
  node->woken = false;
  if (*list)
    (*list)->prev = node;
  *list = node;
}

static void list_remove(struct __waitlist_node **list,
                        struct __waitlist_node *node) {
  if (node->prev)
    node->prev->next = node->next;
  else
    *list = node->next;
  if (node->next)
    node->next->prev = node->prev;
}

/// If waiting indefinitely this is relatively easy, call the `thread.suspend`
/// intrinsic and we won't get resumed until we're actually signaleld.
static void wait_indefinitely(struct __waitlist_node **list) {
  struct __waitlist_node node = {
      .kind = WAITLIST_NODE_TID,
      .tid_or_future = wasip3_thread_index(),
  };
  list_insert(list, &node);
  wasip3_thread_suspend();

  // Another thread may have modified *list (and node.next) while we were
  // suspended. Prevent the compiler from caching any memory values across
  // the suspend point. This is a compiler-only fence: no instructions are
  // emitted, so no atomics feature is required.
  __atomic_signal_fence(memory_order_seq_cst);
}

/// Waiting with a timeout is more complicated than waiting indefinitely. Here
/// a unit future is created along with a waitable set, and the timeout itself
/// comes from `wasi:clocks`. Wait on either the timeout lapsing or the future
/// being notified, returning what happened.
///
/// TODO(wasip3): one optimization that could be applied here is to have a
/// per-thread unit stream and waitable-set which is used to block. That would
/// involve hooking into thread shutdown, however, which is currently left for a
/// future refactoring.
static int wait_timeout(struct __waitlist_node **list, clockid_t clk,
                        const struct timespec *at) {
  wasip3_event_t event;

  // Create a subtask based on sleeping until `at`. Note that if this subtask
  // immediately returns we don't bother with `list` and just immediately return
  // `ETIMEDOUT`.
  wasip3_subtask_status_t status;
  if (clk == CLOCK_MONOTONIC) {
    monotonic_clock_mark_t deadline;
    if (!timespec_to_instant_clamp(at, &deadline))
      return EINVAL;
    status = monotonic_clock_wait_until(deadline);
  } else {
    assert(clk == CLOCK_REALTIME);
    // FIXME(WebAssembly/WASI#857): `CLOCK_REALTIME` is not supported by the
    // WASI interfaces. It's the default for `pthread_*` primitives, however, so
    // it kind of needs to be supported. Calculate the diff in seconds between
    // `at` and `now` and then sleep on the monotonic clock for that amount of
    // time. That's not the right clock, but it's the best we can do.
    system_clock_instant_t now, desired;

    if (!timespec_to_timestamp_clamp(at, &desired))
      return EINVAL;
    system_clock_now(&now);

    // Check if the deadline has passed already, otherwise we can't create a
    // deadline to sleep until.
    if (desired.seconds < now.seconds ||
        (desired.seconds == now.seconds &&
         desired.nanoseconds <= now.nanoseconds))
      return ETIMEDOUT;

    // Calculate `amt = desired - now` while attempting to be quite careful
    // about overflow. Love working in C.
    uint64_t delta_sec = (uint64_t)desired.seconds - (uint64_t)now.seconds;
    int64_t delta_nsec = desired.nanoseconds - (int64_t)now.nanoseconds;
    if (delta_nsec < 0) {
      delta_nsec += NSEC_PER_SEC;
      delta_sec -= 1;
    }
    monotonic_clock_duration_t amt;
    if (__builtin_mul_overflow(delta_sec, NSEC_PER_SEC, &amt) ||
        __builtin_add_overflow(amt, (uint64_t)delta_nsec, &amt))
      amt = UINT64_MAX;

    // Perform a relative sleep now that we know how many ns is desired.
    status = monotonic_clock_wait_for(amt);
  }
  if (WASIP3_SUBTASK_STATE(status) == WASIP3_SUBTASK_RETURNED)
    return ETIMEDOUT;
  assert(WASIP3_SUBTASK_STATE(status) == WASIP3_SUBTASK_STARTING ||
         WASIP3_SUBTASK_STATE(status) == WASIP3_SUBTASK_STARTED);
  wasip3_subtask_t subtask = WASIP3_SUBTASK_HANDLE(status);

  // Setup the future/waitable-set state. This'll get cleaned up on function
  // exit. Note that this also enables a pending read on the future at the same
  // time.
  uint64_t reader_and_writer = future_new_unit();
  uint32_t reader = (uint32_t)reader_and_writer;
  uint32_t writer = (uint32_t)(reader_and_writer >> 32);
  wasip3_waitable_status_t rc = future_read_unit(reader, NULL);
  assert(rc == WASIP3_WAITABLE_STATUS_BLOCKED);

  // Insert ourselves into the waitlist provided.
  struct __waitlist_node node = {
      .kind = WAITLIST_NODE_FUTURE,
      .tid_or_future = writer,
  };
  list_insert(list, &node);

  // Actually block. This will wait until either the future is written to or
  // the deadline of the subtask above is reached.
  //
  // See comments above in `wait_indefinitely` for why this fence is needed.
  uint32_t ws = wasip3_waitable_set_new();
  wasip3_waitable_join(reader, ws);
  wasip3_waitable_join(subtask, ws);
  wasip3_waitable_set_wait(ws, &event);
  __atomic_signal_fence(memory_order_seq_cst);

  // Delegate based on what event happen. Note that each arm cancels the event
  // that didn't happen.
  if (event.event == WASIP3_EVENT_SUBTASK) {
    // If a subtask event happened, that means that the deadline was
    // reached. In that situation we're done with the subtask so discard it.
    assert(event.waitable == subtask);
    assert(event.code == WASIP3_SUBTASK_RETURNED);

    // Cancel the future read since we're no longer interested in the result.
    wasip3_waitable_join(reader, 0);
    rc = future_cancel_read_unit(reader);
    if (node.woken)
      assert(WASIP3_WAITABLE_STATE(rc) == WASIP3_WAITABLE_COMPLETED);
    else
      assert(WASIP3_WAITABLE_STATE(rc) == WASIP3_WAITABLE_CANCELLED);
    assert(WASIP3_WAITABLE_COUNT(rc) == 0);
  } else if (event.event == WASIP3_EVENT_FUTURE_READ) {
    // If a future read event happened then that means we were signaled from
    // another thread. Cancel and then drop the subtask for waiting as we no
    // longer need it.
    assert(event.waitable == reader);
    assert(WASIP3_WAITABLE_STATE(event.code) == WASIP3_WAITABLE_COMPLETED);
    assert(WASIP3_WAITABLE_COUNT(event.code) == 0);

    // Cancel the subtask as we're no longer interested in its result.
    wasip3_waitable_join(subtask, 0);
    status = wasip3_subtask_cancel(subtask);
    assert(WASIP3_SUBTASK_STATE(status) == WASIP3_SUBTASK_RETURNED ||
           WASIP3_SUBTASK_STATE(status) == WASIP3_SUBTASK_STARTED_CANCELLED ||
           WASIP3_SUBTASK_STATE(status) == WASIP3_SUBTASK_RETURNED_CANCELLED);
  } else {
    // Nothing else should show up.
    __builtin_trap();
  }

  // Don't need these any more, clean them up.
  wasip3_subtask_drop(subtask);
  future_drop_readable_unit(reader);
  wasip3_waitable_set_drop(ws);

  // If we weren't woken we're still in the waitlist, so remove ourselves.
  //
  // Additionally this means nothing was written to the write end of the
  // future. The write end can't be dropped without writing, so do the write
  // here to receive the signal that the read end is gone (dropped above) which
  // enables dropping the write next.
  if (!node.woken) {
    list_remove(list, &node);
    rc = future_write_unit(writer, NULL);
    assert(WASIP3_WAITABLE_STATE(rc) == WASIP3_WAITABLE_DROPPED);
    assert(WASIP3_WAITABLE_COUNT(rc) == 0);
  }

  future_drop_writable_unit(writer);

  return node.woken ? 0 : ETIMEDOUT;
}

int __waitlist_wait_on(struct __waitlist_node **list, clockid_t clk,
                       const struct timespec *at) {
  // Delegate to the internal implementation based on the timeout. Note that we
  // always return "woken" for the indefinite wait scenario.
  if (at)
    return wait_timeout(list, clk, at);
  wait_indefinitely(list);
  return 0;
}

static void wake_node(struct __waitlist_node *node, int yield) {
  node->woken = true;
  if (node->kind == WAITLIST_NODE_TID) {
    if (yield) {
      wasip3_thread_yield_then_resume(node->tid_or_future);
    } else {
      wasip3_thread_resume_later(node->tid_or_future);
    }
  } else {
    // Note that `yield` is ignored for future-based (timed) waiters: writing
    // to the future queues an event for the waiter but doesn't transfer
    // control to it. This could be changed if the tid is stored in the node in
    // addition to the future, but for now this is just an optimization anyway
    // so it's just perf not correctness if this is done.
    assert(node->kind == WAITLIST_NODE_FUTURE);
    wasip3_waitable_status_t rc = future_write_unit(node->tid_or_future, NULL);
    assert(WASIP3_WAITABLE_STATE(rc) == WASIP3_WAITABLE_COMPLETED);
    assert(WASIP3_WAITABLE_COUNT(rc) == 0);
  }
}

void __waitlist_wake_one(struct __waitlist_node **list, int yield) {
  if (*list == NULL) {
    return;
  }
  struct __waitlist_node *node = *list;
  list_remove(list, node);
  wake_node(node, yield);
}

void __waitlist_wake_all(struct __waitlist_node **list, int yield) {
  // Detach the entire waitlist up front and iterate over it locally. Waking a
  // thread may yield to it and may modify `list` which we don't want to tamper
  // with.
  struct __waitlist_node *curr = *list;
  *list = NULL;

  while (curr) {
    // Read `next` before waking, since `curr`'s storage may go away once it
    // runs.
    struct __waitlist_node *next = curr->next;
    // As a scheduling optimization, if yielding is allowed, we always yield
    // directly to the last suspended thread instead of just scheduling it to
    // run at some point.
    wake_node(curr, next == NULL && yield);
    curr = next;
  }
}
