// We cannot support futexes in single-threaded mode, so we just trap if any code tries to use them.
#include <wasi/libc.h>
int __wasilibc_futex_wait(volatile int *addr, int val, clockid_t clk,
                          const struct timespec *at, unsigned flags)
{
  __builtin_trap();
}

void __wasilibc_futex_wake(volatile int *addr, int count, unsigned flags)
{
  __builtin_trap();
}