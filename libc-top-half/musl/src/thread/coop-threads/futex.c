#include "common/time.h"
#include "hashmap.h"
#include "pthread_impl.h"
#include <assert.h>
#include <stdatomic.h>
#include <time.h>
#include <wasi/api.h>
#include <wasi/libc.h>
#include "common/clock.h"

#ifndef __wasip3__
#error "Unknown WASI version"
#endif

#define WAITLIST_NODE_TID 0
#define WAITLIST_NODE_FUTURE 1
struct __waitlist_node {
    // One of `WAITLIST_NODE_TID` or `WAITLIST_NODE_FUTURE`.
    uint32_t kind;
    // The thread id if `kind == WAITLIST_NODE_TID`, or a writable future
    // handle if `kind == WAITLIST_NODE_FUTURE`.
    uint32_t tid_or_future;
    // Set to `true` when this node is removed from the linked list by an
    // explicit thread notification.
    bool woken;

    // Linked list pointers for easy insertion/removal on timeout.
    struct __waitlist_node *next;
    struct __waitlist_node *prev;
};

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

/* ------------------------------------------------------------------ */
/* RACE FIX (parallel guest threads).                                  */
/*                                                                     */
/* Guest threads can run in parallel on multiple host threads (the     */
/* embedder schedules fibers on a multi-threaded runtime). The wait /  */
/* wake protocol below originally assumed serialized fibers: between   */
/* a waiter creating its map entry and inserting its waitlist node     */
/* there was no lock and no suspension point, so a waker running in    */
/* that window found an existing entry with an empty waitlist and      */
/* dropped the wake (`woken=0`), leaving the waiter suspended forever. */
/* The map and refcounts were likewise unguarded against concurrent    */
/* same-address waiters.                                               */
/*                                                                     */
/* Fix, in two layers:                                                 */
/*                                                                     */
/* 1. A global spinlock gate (`futex_gate`) serializes every mutation  */
/*    of the futex map, waitlists, refcounts, `woken` flags, and the   */
/*    `*addr` value recheck. Every critical section is free of         */
/*    suspension points (no `thread.suspend`/`thread.yield`, only      */
/*    non-blocking host calls), so bounded spinning is safe: the gate  */
/*    holder always makes progress.                                    */
/*                                                                     */
/* 2. Indefinite waits no longer use TID nodes (`thread.suspend`),     */
/*    because `thread.resume_later` traps on a not-yet-suspended       */
/*    thread, which made wake-before-block unrepresentable. They now   */
/*    use the same unit-future machinery as timed waits: a wake that   */
/*    lands before the waiter blocks is queued as a pending event and  */
/*    the later `waitable_set_wait` returns immediately.               */
/*                                                                     */
/* The `__WASILIBC_FUTEX_YIELD` hand-off is not performed while holding */
/* the gate (`thread.yield_then_resume` would suspend the caller      */
/* inside the critical section); wakes use the non-yielding resume     */
/* path. With future-based waiters the yield flag had no effect on     */
/* correctness anyway.                                                 */
/* ------------------------------------------------------------------ */

static volatile int futex_gate = 0;

static void gate_acquire(void) {
  while (__sync_lock_test_and_set(&futex_gate, 1)) {
    // Cooperatively yield while the gate is held elsewhere. A bare spin
    // loop has no suspension point, so under cooperative scheduling the
    // fiber monopolizes its executor: the holder's pending wake/work items
    // are never processed and the gate is never released. Threads must be
    // rescheduled for the holder to make progress.
    wasip3_thread_yield();
  }
}

static void gate_release(void) {
  __sync_lock_release(&futex_gate);
}

/// Waiting indefinitely uses the same unit-future machinery as the timed
/// wait: the waker writes the future, and a write that lands before we
/// block is queued as a pending event so we never miss it.
///
/// Called with the gate held; re-checks `*addr != val` inside the gate and
/// releases the gate before blocking. The node is inserted while the gate
/// is held so the waker cannot dequeue us in the insert-to-block window.
/// Returns 1 if parked (caller must release the entry reference), 0 if the
/// value already changed (caller returns EWOULDBLOCK-free success).
static int wait_indefinitely(struct __waitlist_node **list,
                             volatile int *addr, int val) {
  if (*addr != val) {
    gate_release();
    return 0;
  }
  uint64_t reader_and_writer = future_new_unit();
  uint32_t reader = (uint32_t)reader_and_writer;
  uint32_t writer = (uint32_t)(reader_and_writer >> 32);
  wasip3_waitable_status_t rc = future_read_unit(reader, NULL);
  assert(rc == WASIP3_WAITABLE_STATUS_BLOCKED);

  struct __waitlist_node node = {
      .kind = WAITLIST_NODE_FUTURE,
      .tid_or_future = writer,
  };
  list_insert(list, &node);
  gate_release();

  uint32_t ws = wasip3_waitable_set_new();
  wasip3_waitable_join(reader, ws);
  wasip3_event_t event;
  wasip3_waitable_set_wait(ws, &event);
  __atomic_signal_fence(memory_order_seq_cst);

  // Only the future can fire: no subtask was joined to the set, and the
  // waker dequeues us (under the gate) before writing the future, so
  // `node` is no longer in the list here and is implicitly woken.
  assert(event.event == WASIP3_EVENT_FUTURE_READ);
  assert(event.waitable == reader);
  assert(WASIP3_WAITABLE_STATE(event.code) == WASIP3_WAITABLE_COMPLETED);
  assert(WASIP3_WAITABLE_COUNT(event.code) == 0);

  // Detach the reader from the set before dropping the set: a waitable set
  // must be empty of joined waitables when dropped, and the reader still
  // holds its `set` back-reference until `future.drop-readable` clears it.
  future_drop_readable_unit(reader);
  wasip3_waitable_set_drop(ws);
  future_drop_writable_unit(writer);
  return 1;
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
///
/// Called WITHOUT the gate held (subtask creation below suspends the fiber,
/// and the gate must never be held across a suspension point). The gate is
/// taken only for the value recheck + node insertion before blocking and for
/// the timeout-path reconciliation, both of which are suspension-free.
/// `val` is re-checked at insertion so a wake that landed while we were
/// creating the subtask can never be missed.
static int wait_timeout(struct __waitlist_node **list, volatile int *addr,
                        int val, clockid_t clk, const struct timespec *at,
                        unsigned flags) {
  wasip3_event_t event;

  // Create a subtask based on sleeping until `at`. Note that if this subtask
  // immediately returns we don't bother with `list` and just immediately return
  // `ETIMEDOUT`. No gate is held here: creating the subtask can suspend this
  // fiber.
  wasip3_subtask_status_t status;
  if (clk->id == CLOCKID_MONOTONIC || !(flags & __WASILIBC_FUTEX_TIMESPEC_ABSOLUTE)) {
    monotonic_clock_mark_t deadline;
    if (!timespec_to_instant_clamp(at, &deadline))
      return EINVAL;
    if (flags & __WASILIBC_FUTEX_TIMESPEC_ABSOLUTE)  {
      status = monotonic_clock_wait_until(deadline);
    } else {
      status = monotonic_clock_wait_for(deadline);
    }
  } else {
    assert(clk->id == CLOCKID_REALTIME);
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
  // time. These host calls are suspension-free.
  uint64_t reader_and_writer = future_new_unit();
  uint32_t reader = (uint32_t)reader_and_writer;
  uint32_t writer = (uint32_t)(reader_and_writer >> 32);
  wasip3_waitable_status_t rc = future_read_unit(reader, NULL);
  assert(rc == WASIP3_WAITABLE_STATUS_BLOCKED);

  // Insert ourselves into the waitlist under the gate, re-checking the futex
  // value in the same critical section: if a waker stored the new value while
  // we were creating the subtask above, its wake could not have reached us
  // (we were not in the list), so we must not sleep.
  struct __waitlist_node node = {
      .kind = WAITLIST_NODE_FUTURE,
      .tid_or_future = writer,
  };
  gate_acquire();
  if (*addr != val) {
    gate_release();
    // Tear down the subtask and the future; nobody can be writing our
    // future (we were never in the waitlist).
    wasip3_waitable_join(subtask, 0);
    status = wasip3_subtask_cancel(subtask);
    assert(WASIP3_SUBTASK_STATE(status) == WASIP3_SUBTASK_RETURNED ||
           WASIP3_SUBTASK_STATE(status) == WASIP3_SUBTASK_STARTED_CANCELLED ||
           WASIP3_SUBTASK_STATE(status) == WASIP3_SUBTASK_RETURNED_CANCELLED);
    wasip3_subtask_drop(subtask);
    future_drop_readable_unit(reader);
    rc = future_write_unit(writer, NULL);
    assert(WASIP3_WAITABLE_STATE(rc) == WASIP3_WAITABLE_DROPPED);
    assert(WASIP3_WAITABLE_COUNT(rc) == 0);
    future_drop_writable_unit(writer);
    return 0;
  }
  list_insert(list, &node);
  gate_release();

  // Actually block. This will wait until either the future is written to or
  // the deadline of the subtask above is reached. A wake that already landed
  // is queued as a pending event, so this returns immediately in that case.
  //
  // See `wait_indefinitely` for why this fence is needed.
  uint32_t ws = wasip3_waitable_set_new();
  wasip3_waitable_join(reader, ws);
  wasip3_waitable_join(subtask, ws);
  wasip3_waitable_set_wait(ws, &event);
  __atomic_signal_fence(memory_order_seq_cst);

  // Delegate based on what event happened. Note that each arm cancels the event
  // that didn't happen.
  if (event.event == WASIP3_EVENT_SUBTASK) {
    // If a subtask event happened, that means that the deadline was
    // reached. In that situation we're done with the subtask so discard it.
    assert(event.waitable == subtask);
    assert(event.code == WASIP3_SUBTASK_RETURNED);

    // Reconcile with the waitlist under the gate first: either a waker
    // already dequeued us (its whole dequeue + `woken` + future write is one
    // gate-held step), or we remove ourselves now. Either way, after this
    // critical section our node is out of the list and no waker can touch
    // our future any more — so the cancel below is deterministic: COMPLETED
    // iff the waker's write already landed, CANCELLED otherwise.
    gate_acquire();
    bool woken = node.woken;
    if (!woken)
      list_remove(list, &node);
    gate_release();

    wasip3_waitable_join(reader, 0);
    rc = future_cancel_read_unit(reader);
    if (woken)
      assert(WASIP3_WAITABLE_STATE(rc) == WASIP3_WAITABLE_COMPLETED);
    else
      assert(WASIP3_WAITABLE_STATE(rc) == WASIP3_WAITABLE_CANCELLED);
    assert(WASIP3_WAITABLE_COUNT(rc) == 0);

    // Don't need these any more, clean them up.
    wasip3_subtask_drop(subtask);
    future_drop_readable_unit(reader);
    wasip3_waitable_set_drop(ws);

    // If we weren't woken nothing was written to the write end of the
    // future. The write end can't be dropped without writing, and the read
    // end is gone (dropped above), so do the write here to resolve it as
    // DROPPED. No waker can be blocked writing this future: a waker only
    // writes futures whose node it dequeued, which implies `woken`.
    if (!woken) {
      rc = future_write_unit(writer, NULL);
      assert(WASIP3_WAITABLE_STATE(rc) == WASIP3_WAITABLE_DROPPED);
      assert(WASIP3_WAITABLE_COUNT(rc) == 0);
    }

    future_drop_writable_unit(writer);

    return woken ? 0 : ETIMEDOUT;
  } else if (event.event == WASIP3_EVENT_FUTURE_READ) {
    // If a future read event happened then that means we were signaled from
    // another thread. The waker already dequeued us (under the gate) before
    // writing the future, so there is nothing to reconcile in the waitlist.
    // Cancel and then drop the subtask for waiting as we no longer need it.
    assert(event.waitable == reader);
    assert(WASIP3_WAITABLE_STATE(event.code) == WASIP3_WAITABLE_COMPLETED);
    assert(WASIP3_WAITABLE_COUNT(event.code) == 0);

    // Cancel the subtask as we're no longer interested in its result.
    wasip3_waitable_join(subtask, 0);
    status = wasip3_subtask_cancel(subtask);
    assert(WASIP3_SUBTASK_STATE(status) == WASIP3_SUBTASK_RETURNED ||
           WASIP3_SUBTASK_STATE(status) == WASIP3_SUBTASK_STARTED_CANCELLED ||
           WASIP3_SUBTASK_STATE(status) == WASIP3_SUBTASK_RETURNED_CANCELLED);

    // Don't need these any more, clean them up. The write end was written
    // by the waker, so it can be dropped directly.
    wasip3_subtask_drop(subtask);
    future_drop_readable_unit(reader);
    wasip3_waitable_set_drop(ws);
    future_drop_writable_unit(writer);

    return 0;
  } else {
    // Nothing else should show up.
    __builtin_trap();
  }
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
    // Note that `yield` is ignored for future-based waiters: writing to the
    // future queues an event for the waiter but doesn't transfer control to
    // it. This is just an optimization anyway, so it's perf not correctness.
    assert(node->kind == WAITLIST_NODE_FUTURE);
    wasip3_waitable_status_t rc = future_write_unit(node->tid_or_future, NULL);
    assert(WASIP3_WAITABLE_STATE(rc) == WASIP3_WAITABLE_COMPLETED);
    assert(WASIP3_WAITABLE_COUNT(rc) == 0);
  }
}

// We supply a futex-style interface on top of our cooperative threading model to
// maintain compatibility with the existing pthreads ABI. This is supported by a
// hashmap of futex addresses to waitlists of threads that are waiting on those
// futexes.
//
// Note that the waitlist of each entry is separately heap-allocated. This
// ensures that the list itself is stable across hash map reallocations which
// enables `wait_timeout` and `wait_indefinitely` to rely on the list being
// valid before and after the operation.
struct __futex_waitlist {
  struct __waitlist_node *list;
  size_t refcnt;
};

struct __futex_entry {
  volatile int *addr;
  struct __futex_waitlist *waiters;
};

// The futex map should be accessed through `get_futex_map` to ensure it is lazily initialized. It is never freed.
static struct hashmap *futex_map;

// The hash function and comparison functions are supplied to the hashmap to allow it to store `__futex_entry`
// structs keyed by their `addr` field.
static uint64_t futex_entry_hash(const void *item, uint64_t seed0,
                                 uint64_t seed1) {
  const struct __futex_entry *entry = item;
  return __hashmap_xxhash3(&entry->addr, sizeof(entry->addr), seed0, seed1);
}
static int futex_entry_compare(const void *a, const void *b, void *udata) {
  const struct __futex_entry *left = a;
  const struct __futex_entry *right = b;
  (void)udata;
  return left->addr != right->addr;
}

static struct hashmap *get_futex_map(bool create) {
  if (futex_map || !create)
    return futex_map;

  const size_t initial_capacity = 64;
  futex_map = __hashmap_new(sizeof(struct __futex_entry), initial_capacity, 0, 0,
                          futex_entry_hash, futex_entry_compare, NULL, NULL);
  return futex_map;
}

static struct __futex_waitlist *find_futex_entry(volatile int *addr,
                                                bool create) {
  struct hashmap *map = get_futex_map(create);

  // Create a temporary key to search for the futex entry in the hashmap;
  // the `waiters` field is not used for comparison, so it can be NULL.
  struct __futex_entry key = {
      .addr = addr,
      .waiters = NULL,
  };

  if (!map)
    return NULL;

  struct __futex_entry *entry = (struct __futex_entry *)__hashmap_get(map, &key);
  if (entry)
    return entry->waiters;
  if (!create)
    return NULL;

  key.waiters = malloc(sizeof(struct __futex_waitlist));
  if (!key.waiters)
    return NULL;
  key.waiters->list = NULL;
  key.waiters->refcnt = 0;

  if (!__hashmap_set(map, &key) && __hashmap_oom(map)) {
    free(key.waiters);
    return NULL;
  }

  return key.waiters;
}

// Drops this waiter's reference to `waiters`, removing the futex's entry from
// the hashmap and deallocating it once nothing is waiting on the address any
// more. Takes the gate: concurrent waiters on the same address can exit
// simultaneously, and the refcount decrement, map removal and free must not
// interleave with a waker dereferencing the entry.
static void release_futex_entry(volatile int *addr,
                                struct __futex_waitlist *waiters) {
  gate_acquire();
  if (--waiters->refcnt > 0) {
    gate_release();
    return;
  }
  assert(waiters->list == NULL);
  struct __futex_entry key = {
      .addr = addr,
      .waiters = NULL,
  };
  __hashmap_delete(get_futex_map(false), &key);
  free(waiters);
  gate_release();
}

int __wasilibc_futex_wait(volatile int *addr, int val, clockid_t clk,
                          const struct timespec *at, unsigned flags)
{
  if ((flags & ~__WASILIBC_FUTEX_TIMESPEC_ABSOLUTE) != 0) {
    return -EINVAL;
  }
  if ((((intptr_t)addr) & 3) != 0) {
    return -EINVAL;
  }
  if (*addr != val) {
    return -EWOULDBLOCK;
  }

  if (at) {
    if (at->tv_nsec >= 1000000000UL)
      return -EINVAL;
    if (at->tv_nsec < 0)
      return -EINVAL;
    if (!(flags & __WASILIBC_FUTEX_TIMESPEC_ABSOLUTE) && at->tv_sec < 0)
      return -EINVAL;
  }

  gate_acquire();
  // Re-check the value under the gate: if the waking thread already stored
  // the new value before we got here, its wake found no waiter of ours and
  // we must not sleep.
  if (*addr != val) {
    gate_release();
    return 0;
  }

  struct __futex_waitlist *waiters = find_futex_entry(addr, true);
  if (!waiters) {
    gate_release();
    return ENOMEM;
  }
  waiters->refcnt++;
  struct __waitlist_node **list = &waiters->list;

  int rc = 0;
  if (at) {
    // `wait_timeout` manages its own gate: creating the clock subtask below
    // suspends this fiber, and the gate must never be held across a
    // suspension. It re-takes the gate for the value recheck + insertion
    // right before blocking.
    gate_release();
    rc = wait_timeout(list, addr, val, clk, at, flags);
  } else {
    if (!wait_indefinitely(list, addr, val)) {
      // Value changed while we were (re-)checking under the gate; the wake
      // is not missed, return without parking. `rc` stays 0.
      rc = 0;
    }
  }
  release_futex_entry(addr, waiters);
  return rc ? -rc : 0;
}

int __wasilibc_futex_wake(volatile int *addr, int count, unsigned flags)
{
  if (flags != 0 && flags != __WASILIBC_FUTEX_YIELD) {
    return -EINVAL;
  }
  int yield = (flags & __WASILIBC_FUTEX_YIELD) != 0;
  volatile int *word = (volatile int *)addr;

  gate_acquire();

  struct __futex_waitlist *waiters = find_futex_entry(word, false);
  if (!waiters) {
    gate_release();
    return 0;
  }
  struct __waitlist_node **list = &waiters->list;

  count = (count < 0) ? INT_MAX : count;

  // "Atomically" dequeue all waiters first, then wake them all up. That way if
  // a waiter re-enqueues themselves we won't see it. Everything up to and
  // including the wake itself happens under the gate: a timed waiter's cleanup
  // reconciles against `woken` under the same gate, so dequeue, `woken` and
  // the future write are one atomic step from the waiter's point of view.
  // All calls here are non-suspending.
  struct __waitlist_node *to_wake = NULL;
  while (count > 0 && *list) {
    struct __waitlist_node *node = *list;
    list_remove(list, node);
    list_insert(&to_wake, node);
    count--;
  }

  // Wake all dequeued waiters. The `yield` hand-off is not performed while
  // holding the gate (`thread.yield_then_resume` would suspend the caller
  // inside the critical section); with future-based waiters it had no
  // correctness effect anyway.
  int woken = 0;
  while (to_wake) {
    struct __waitlist_node *node = to_wake;
    list_remove(&to_wake, node);
    wake_node(node, 0);
    woken++;
  }
  gate_release();
  (void)yield;
  return woken;
}
