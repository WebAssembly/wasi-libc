#include "pthread_impl.h"

int __pthread_rwlock_timedrdlock(pthread_rwlock_t *restrict rw,
                                 const struct timespec *restrict at) {
  /* Fast path: if no writer, increment readers */
  if (rw->_rw_lock >= 0) {
    rw->_rw_lock++;
    return 0;
  }

  /* Would need to wait for writer, but timeouts not supported */
  if (at) {
    errno = ENOSYS;
    return -1;
  }

  /* Wait until no writer holds the lock */
  while (rw->_rw_lock < 0) {
    __waitlist_wait_on(&rw->_rw_waiters);
  }

  /* Acquired, increment reader count */
  rw->_rw_lock++;
  return 0;
}

weak_alias(__pthread_rwlock_timedrdlock, pthread_rwlock_timedrdlock);
