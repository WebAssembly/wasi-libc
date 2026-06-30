#include "pthread_impl.h"

int __wasilibc_pthread_mutex_unlock(pthread_mutex_t *m, int yield) {
  int tid = wasip3_thread_index();
  int type = m->_m_type & 15;

  /* Check ownership */
  if (m->_m_lock != tid) {
    if (type == PTHREAD_MUTEX_ERRORCHECK || type == PTHREAD_MUTEX_RECURSIVE) {
      return EPERM;
    }
    /* Normal mutex: undefined behavior */
    __builtin_trap();
  }

  /* Handle recursive unlock */
  if (type == PTHREAD_MUTEX_RECURSIVE && m->_m_count > 1) {
    m->_m_count--;
    return 0;
  }

  /* Unlock */
  m->_m_lock = 0;
  m->_m_count = 0;
  __waitlist_wake_one(&m->_m_waiters, yield);
  return 0;
}

int __pthread_mutex_unlock(pthread_mutex_t *m) {
  return __wasilibc_pthread_mutex_unlock(m, 1);
}

weak_alias(__pthread_mutex_unlock, pthread_mutex_unlock);
