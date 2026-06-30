#include "pthread_impl.h"
#include <threads.h>

int mtx_trylock(mtx_t *m) {
  if (m->_m_type == PTHREAD_MUTEX_NORMAL) {
    if (m->_m_lock == 0) {
      m->_m_lock = EBUSY;
      return thrd_success;
    } else {
      return thrd_busy;
    }
  }
  int ret = __pthread_mutex_trylock((pthread_mutex_t *)m);
  switch (ret) {
  default:
    return thrd_error;
  case 0:
    return thrd_success;
  case EBUSY:
    return thrd_busy;
  }
}
