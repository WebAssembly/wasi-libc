// We cannot support futexes in single-threaded mode, so we just trap if we need to wait, and make waking a no-op.

#include <wasi/libc.h>
#include <wasi/api.h>
#include "common/time.h"
#include <errno.h>
#include <time.h>

int __wasilibc_futex_wait(volatile int *addr, int val, clockid_t clk,
                          const struct timespec *at, unsigned flags)
{
  if ((flags & ~__WASILIBC_FUTEX_TIMESPEC_ABSOLUTE) != 0) {
    return -EINVAL;
  }
  if ((((intptr_t)addr) & 3) != 0) {
    return -EINVAL;
  }
  if (*addr != val) {
    return -EWOULDBLOCK;
  }

  if (at) {
    if (at->tv_nsec >= 1000000000UL)
      return -EINVAL;
    if (at->tv_nsec < 0)
      return -EINVAL;

    if (flags & __WASILIBC_FUTEX_TIMESPEC_ABSOLUTE) {
      struct timespec to;
      if (__clock_gettime(clk, &to))
        return -EINVAL;
      to.tv_sec = at->tv_sec - to.tv_sec;
      if ((to.tv_nsec = at->tv_nsec - to.tv_nsec) < 0) {
        to.tv_sec--;
        to.tv_nsec += 1000000000;
      }
      if (to.tv_sec < 0)
        return -ETIMEDOUT;
    } else {
      if (at->tv_sec < 0)
        return -EINVAL;
    }
  }
  __builtin_trap();
}

int __wasilibc_futex_wake(volatile int *addr, int count, unsigned flags)
{
  (void)addr;
  (void)count;
  if (flags != 0 && flags != __WASILIBC_FUTEX_YIELD) {
    return -EINVAL;
  }
  // no-op
  return 0;
}
