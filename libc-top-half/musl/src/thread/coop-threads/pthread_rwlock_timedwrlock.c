#include "pthread_impl.h"
#include <time.h>

int __pthread_rwlock_timedwrlock(pthread_rwlock_t *restrict rw,
                                 const struct timespec *restrict at) {
  int tid = wasip3_thread_index();

  /* Check for deadlock: trying to write-lock already owned write lock */
  if (rw->_rw_lock == -tid) {
    return EDEADLK;
  }

  /* Fast path: if no readers or writers, acquire write lock */
  if (rw->_rw_lock == 0) {
    rw->_rw_lock = -tid; /* -tid indicates write lock owned by this thread */
    return 0;
  }

  /* Wait until no readers and no writers */
  while (rw->_rw_lock != 0) {
    int v = rw->_rw_lock;
    int rc = __timedwait(&rw->_rw_lock, v, CLOCK_REALTIME, at, 1);
    if (rc != 0)
      return rc;
  }

  /* Acquired, mark as write-locked with owner */
  rw->_rw_lock = -tid;
  return 0;
}

weak_alias(__pthread_rwlock_timedwrlock, pthread_rwlock_timedwrlock);
