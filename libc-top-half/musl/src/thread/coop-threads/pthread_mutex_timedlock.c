#include "pthread_impl.h"
#include <time.h>

int __pthread_mutex_timedlock(pthread_mutex_t *restrict m,
                              const struct timespec *restrict at) {
  int tid = wasip3_thread_index();
  int type = m->_m_type & 15;

  /* Fast path: if available, acquire regardless of timeout */
  if (m->_m_lock == 0) {
    m->_m_lock = tid;
    m->_m_count = 1;
    return 0;
  }

  /* Check for recursive lock */
  if (m->_m_lock == tid) {
    if (type == PTHREAD_MUTEX_RECURSIVE) {
      m->_m_count++;
      return 0;
    }
    if (type == PTHREAD_MUTEX_ERRORCHECK) {
      return EDEADLK;
    }
    /* Normal mutex: deadlock */
    __builtin_trap();
  }

  /* Wait for the lock to be available */
  while (m->_m_lock != 0) {
    int rc = __waitlist_wait_on(&m->_m_waiters, CLOCK_REALTIME, at);
    if (rc != 0)
      return rc;
  }

  m->_m_lock = tid;
  m->_m_count = 1;
  return 0;
}

weak_alias(__pthread_mutex_timedlock, pthread_mutex_timedlock);
