// We cannot support futexes in single-threaded mode, so we just trap if we need to wait, and make waking a no-op.

#include <wasi/libc.h>
#include <wasi/api.h>
#include "common/time.h"
#include <errno.h>
#include <time.h>

int __wasilibc_futex_wait(volatile int *addr, int val, clockid_t clk,
                          const struct timespec *at, unsigned flags)
{
  (void)clk;
  (void)at;
  if (flags != 0) {
    return -EINVAL;
  }
  if (*addr != val) {
    return -EWOULDBLOCK;
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