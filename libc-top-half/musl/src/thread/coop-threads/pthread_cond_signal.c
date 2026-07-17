#include "pthread_impl.h"

int pthread_cond_signal(pthread_cond_t *c) {
  __wake(&c->_c_waiters, 1, 1);
  return 0;
}
