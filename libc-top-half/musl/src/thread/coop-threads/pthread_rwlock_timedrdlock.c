#include "pthread_impl.h"
#include <time.h>

int __pthread_rwlock_timedrdlock(pthread_rwlock_t *restrict rw,
                                 const struct timespec *restrict at) {
  /* Fast path: if no writer, increment readers */
  if (rw->_rw_lock >= 0) {
    rw->_rw_lock++;
    return 0;
  }

  /* Wait until no writer holds the lock */
  while (rw->_rw_lock < 0) {
    int rc = __waitlist_wait_on(&rw->_rw_waiters, CLOCK_REALTIME, at);
    if (rc != 0)
      return rc;
  }

  /* Acquired, increment reader count */
  rw->_rw_lock++;
  return 0;
}

weak_alias(__pthread_rwlock_timedrdlock, pthread_rwlock_timedrdlock);
