#include "pthread_impl.h"
#include <assert.h>
#include <time.h>
enum {
  PTHREAD_ONCE_STATE_NOT_RUN = 0,
  PTHREAD_ONCE_STATE_RUNNING = 1,
  PTHREAD_ONCE_STATE_COMPLETED = 2
};

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
     * see state PTHREAD_ONCE_STATE_RUNNING and wait on this state word. */
    init();

    *control = PTHREAD_ONCE_STATE_COMPLETED;
    __wake(control, -1, 1);
    return 0;
  }

  /* Another thread is initializing, wait for completion */
  while (*control == PTHREAD_ONCE_STATE_RUNNING) {
    int rc = __timedwait(control, PTHREAD_ONCE_STATE_RUNNING, CLOCK_REALTIME,
                         NULL, 1);
    (void)rc;
    assert(rc == 0);
  }

  return 0;
}

weak_alias(__pthread_once, pthread_once);
