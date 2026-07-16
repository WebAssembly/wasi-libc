#include "pthread_impl.h"

int pthread_cond_broadcast(pthread_cond_t *c) {
  __wake(&c->_c_waiters, -1, 1);
  return 0;
}
