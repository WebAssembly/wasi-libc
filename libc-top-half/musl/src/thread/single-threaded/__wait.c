// We cannot support futexes in single-threaded mode, so we just trap if we need to wait, and make waking a no-op.

#include <wasi/libc.h>
#include <wasi/api.h>
#include "common/time.h"
#include <errno.h>
#include <time.h>

static bool timespec_is_past(clockid_t clk, const struct timespec *ts) {
  if (clk == CLOCK_MONOTONIC) {
    monotonic_clock_instant_t now, desired;
    now = monotonic_clock_now();
    if (!timespec_to_instant_clamp(ts, &desired))
      return false;
    if (now >= desired)
      return true;
  }
  if (clk == CLOCK_REALTIME) {
    system_clock_instant_t now, desired;
    system_clock_now(&now);
    if (!timespec_to_timestamp_clamp(ts, &desired))
      return false;
    if (desired.seconds < now.seconds ||
        (desired.seconds == now.seconds &&
         desired.nanoseconds <= now.nanoseconds))
      return true;
  }
  return false;
}

int __wasilibc_futex_wait(volatile int *addr, int val, clockid_t clk,
                          const struct timespec *at, unsigned flags)
{
  (void)flags;

  if (*addr != val) {
    return -EWOULDBLOCK;
  }
  if (at && timespec_is_past(clk, at)) {
    return -ETIMEDOUT;
  }
  __builtin_trap();
}

void __wasilibc_futex_wake(volatile int *addr, int count, unsigned flags)
{
  (void)addr;
  (void)count;
  (void)flags;
  // no-op
}