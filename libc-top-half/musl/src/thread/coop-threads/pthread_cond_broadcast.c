#include "pthread_impl.h"

int pthread_cond_broadcast(pthread_cond_t *c) {
  __waitlist_wake_all(&c->_c_waiters);
  return 0;
}
