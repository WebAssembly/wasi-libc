#include "pthread_impl.h"
#include <stdatomic.h>
#include <wasi/api.h>

#ifndef __wasip3__
#error "Unknown WASI version"
#endif

void __waitlist_wait_on(struct __waitlist_node **list) {
  struct __waitlist_node node = {
      .tid = wasip3_thread_index(),
      .next = *list,
  };
  *list = &node;

  wasip3_thread_suspend();

  // Another thread may have modified *list (and node.next) while we were
  // suspended. Prevent the compiler from caching any memory values across
  // the suspend point. This is a compiler-only fence: no instructions are
  // emitted, so no atomics feature is required.
  __atomic_signal_fence(memory_order_seq_cst);
}

void __waitlist_wake_one(struct __waitlist_node **list, int yield) {
  if (*list == NULL) {
    return;
  }
  struct __waitlist_node *node = *list;
  *list = node->next;
  if (yield) {
    wasip3_thread_yield_then_resume(node->tid);
  } else {
    wasip3_thread_resume_later(node->tid);
  }
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
    if (next == NULL && yield) {
      wasip3_thread_yield_then_resume(curr->tid);
    } else {
      wasip3_thread_resume_later(curr->tid);
    }
    curr = next;
  }
}
