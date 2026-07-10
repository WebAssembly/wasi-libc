#include "pthread_impl.h"
#include <assert.h>

int __pthread_mutex_lock(pthread_mutex_t *m) {
  int tid = wasip3_thread_index();
  int type = m->_m_type & 15;

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

  /* Wait until unlocked */
  while (m->_m_lock != 0) {
    int rc = __waitlist_wait_on(&m->_m_waiters, NULL, NULL);
    (void)rc;
    assert(rc == 0);
  }

  m->_m_lock = tid;
  m->_m_count = 1;
  return 0;
}

weak_alias(__pthread_mutex_lock, pthread_mutex_lock);
