#include "pthread_impl.h"

int __pthread_mutex_trylock(pthread_mutex_t *m) {
  int tid = wasip3_thread_index();
  int type = m->_m_type & 15;

  /* Check for recursive lock */
  if (m->_m_lock == tid) {
    if (type == PTHREAD_MUTEX_RECURSIVE) {
      if (m->_m_count >= INT_MAX)
        return EAGAIN;
      m->_m_count++;
      return 0;
    }
    if (type == PTHREAD_MUTEX_ERRORCHECK) {
      return EBUSY;
    }
    /* Normal mutex: already locked */
    return EBUSY;
  }

  /* Try to acquire the lock */
  if (m->_m_lock != 0) {
    return EBUSY;
  }
  m->_m_lock = tid;
  m->_m_count = 1;

  return 0;
}

weak_alias(__pthread_mutex_trylock, pthread_mutex_trylock);
