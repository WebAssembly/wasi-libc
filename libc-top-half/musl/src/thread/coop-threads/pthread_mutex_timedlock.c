#include "pthread_impl.h"

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

  /* Would need to block, but timeouts not supported */
  if (at) {
    return ETIMEDOUT;
  }

  /* Wait indefinitely */
  while (m->_m_lock != 0) {
    __waitlist_wait_on(&m->_m_waiters);
  }

  m->_m_lock = tid;
  m->_m_count = 1;
  return 0;
}

weak_alias(__pthread_mutex_timedlock, pthread_mutex_timedlock);
