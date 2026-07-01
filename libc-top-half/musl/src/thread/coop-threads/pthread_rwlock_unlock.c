#include "pthread_impl.h"

int __pthread_rwlock_unlock(pthread_rwlock_t *rw) {
  if (rw->_rw_lock > 0) {
    /* Releasing read lock */
    rw->_rw_lock--;
    if (rw->_rw_lock == 0) {
      /* Last reader, wake waiting writers */
      __waitlist_wake_all(&rw->_rw_waiters, 1);
    }
  } else if (rw->_rw_lock < 0) {
    /* Releasing write lock (stored as -tid) */
    rw->_rw_lock = 0;
    /* Wake all waiters (readers and writers compete) */
    __waitlist_wake_all(&rw->_rw_waiters, 1);
  } else {
    /* Unlocking when not locked - trap */
    __builtin_trap();
  }

  return 0;
}

weak_alias(__pthread_rwlock_unlock, pthread_rwlock_unlock);
