#include "pthread_impl.h"

int __pthread_cond_timedwait(pthread_cond_t *restrict c,
                             pthread_mutex_t *restrict m,
                             const struct timespec *restrict ts) {
  // TODO(wasip3) timed waits
  if (ts) {
    return ETIMEDOUT;
  }

  // Specifically do not yield to other threads as part of this unlock which
  // ensures that this thread keeps running into below where it's queued up on
  // the list of waiters which then yields.
  __wasilibc_pthread_mutex_unlock(m, 0);
  __waitlist_wait_on(&c->_c_waiters);
  return pthread_mutex_lock(m);
}

weak_alias(__pthread_cond_timedwait, pthread_cond_timedwait);
