#include "pthread_impl.h"
#include <assert.h>
#include <common/clock.h>
#include <time.h>

int __pthread_cond_timedwait(pthread_cond_t *restrict c,
                             pthread_mutex_t *restrict m,
                             const struct timespec *restrict ts) {
  // The deadline is measured against the clock configured with
  // `pthread_condattr_setclock`, CLOCK_REALTIME by default (note that the
  // static initializer zeroes `_c_clock` and `CLOCKID_REALTIME` is 0).
  assert(CLOCKID_REALTIME == 0);
  clockid_t clk =
      c->_c_clock == CLOCKID_MONOTONIC ? CLOCK_MONOTONIC : CLOCK_REALTIME;

  // Specifically do not yield to other threads as part of this unlock which
  // ensures that this thread keeps running into below where it's queued up on
  // the list of waiters which then yields.
  int rc = __wasilibc_pthread_mutex_unlock(m, 0);
  if (rc != 0)
    return rc;
  // In coop mode this field is used as the futex-style wait key
  // for the internal wait-map, not as a precise waiter count.
  c->_c_waiters = 1;
  int result = __timedwait(&c->_c_waiters, 1, clk, ts, 1);
  rc = pthread_mutex_lock(m);
  (void)rc;
  assert(rc == 0);
  return result;
}

weak_alias(__pthread_cond_timedwait, pthread_cond_timedwait);
