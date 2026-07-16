#include "pthread_impl.h"
#include <assert.h>
#include <time.h>

int pthread_barrier_wait(pthread_barrier_t *b) {
  // Trivial case: only one thread needs to reach barrier
  if (b->_b_limit == 1) {
    return PTHREAD_BARRIER_SERIAL_THREAD;
  }

  // Increment count of waiting threads
  b->_b_count++;

  // If all threads have arrived, wake them all
  if (b->_b_count == b->_b_limit) {
    b->_b_count = 0;
    __wake(&b->_b_count, -1, 1);
    return PTHREAD_BARRIER_SERIAL_THREAD;
  }

  // Otherwise, wait until the last thread resets the barrier count to zero.
  while (b->_b_count != 0) {
    int v = b->_b_count;
    int rc = __timedwait(&b->_b_count, v, CLOCK_REALTIME, NULL, 1);
    (void)rc;
    assert(rc == 0);
  }
  return 0;
}
