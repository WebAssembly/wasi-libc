#include "pthread_impl.h"
#include <assert.h>
enum {
  PTHREAD_ONCE_STATE_NOT_RUN = 0,
  PTHREAD_ONCE_STATE_RUNNING = 1,
  PTHREAD_ONCE_STATE_COMPLETED = 2
};

// For now this implementation has one global list of all waiters for all
// currently active `pthread_once` calls. Ideally this would be a
// per-`pthread_once_t` list  or `pthread_once` would bottom out in something
// that looked like a futex, but for now this is at least a correct
// implementation albeit not the most efficient one.
static struct __waitlist_node *all_waiters;

int __pthread_once(pthread_once_t *control, void (*init)(void)) {
  /* Fast path: already completed */
  if (*control == PTHREAD_ONCE_STATE_COMPLETED) {
    return 0;
  }

  /* Try to become the initializer */
  if (*control == PTHREAD_ONCE_STATE_NOT_RUN) {
    *control = PTHREAD_ONCE_STATE_RUNNING;

    /* init may do something that blocks, in which case other threads
     * may try to run pthread_once on the same control. They will
     * see state PTHREAD_ONCE_STATE_RUNNING and wait on our waitlist. */
    init();

    *control = PTHREAD_ONCE_STATE_COMPLETED;
    __waitlist_wake_all(&all_waiters, 1);
    return 0;
  }

  /* Another thread is initializing, wait for completion */
  while (*control == PTHREAD_ONCE_STATE_RUNNING) {
    int rc = __waitlist_wait_on(&all_waiters, NULL, NULL);
    (void)rc;
    assert(rc == 0);
  }

  return 0;
}

weak_alias(__pthread_once, pthread_once);
