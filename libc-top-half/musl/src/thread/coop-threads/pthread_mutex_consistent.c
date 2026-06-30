#include "atomic.h"
#include "pthread_impl.h"

int pthread_mutex_consistent(pthread_mutex_t *m) {
  // Robust mutexes are not supported in cooperative threading mode
  return ENOTSUP;
}
