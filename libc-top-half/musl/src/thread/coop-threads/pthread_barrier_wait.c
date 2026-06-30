#include "pthread_impl.h"

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
    __waitlist_wake_all(&b->_b_waiters);
    return PTHREAD_BARRIER_SERIAL_THREAD;
  }

  // Otherwise, wait for remaining threads to enter
  __waitlist_wait_on(&b->_b_waiters);
  return 0;
}
