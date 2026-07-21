#include "pthread_impl.h"
#include "assert.h"

#include <wasi/libc.h>

weak int __wasilibc_futex_wait_maybe_busy(volatile void *addr, int val, int64_t max_wait_ns);

// Use WebAssembly's `wait` instruction to implement a futex. Note that `op` is
// unused but retained as a parameter to match the original signature of the
// syscall and that, for `max_wait_ns`, -1 (or any negative number) means wait
// indefinitely.
//
// Adapted from Emscripten: see
// https://github.com/emscripten-core/emscripten/blob/058a9fff/system/lib/pthread/emscripten_futex_wait.c#L111-L150.
int __wasilibc_futex_wait_atomic_wait(volatile void *addr, int val, int64_t max_wait_ns)
{
    int ret = __builtin_wasm_memory_atomic_wait32((int *)addr, val, max_wait_ns);

    // memory.atomic.wait32 returns:
    //   0 => "ok", woken by another agent.
    //   1 => "not-equal", loaded value != expected value
    //   2 => "timed-out", the timeout expired
    if (ret == 1) {
        return -EWOULDBLOCK;
    }
    if (ret == 2) {
        return -ETIMEDOUT;
    }
    assert(ret == 0);
    return 0;
}

int __wasilibc_futex_wait(volatile int *addr, int val, clockid_t clk,
	                  const struct timespec *at, unsigned flags)
{
  int r;

  if ((flags & ~__WASILIBC_FUTEX_TIMESPEC_ABSOLUTE) != 0) {
    return -EINVAL;
  }
  if ((((intptr_t)addr) & 3) != 0) {
    return -EINVAL;
  }
  if (*addr != val) {
    return -EWOULDBLOCK;
  }

  int64_t max_wait_ns = -1;
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
      max_wait_ns = (int64_t)(to.tv_sec * 1000000000 + to.tv_nsec);
    } else {
      if (at->tv_sec < 0)
        return -EINVAL;
      max_wait_ns = (int64_t)(at->tv_sec * 1000000000 + at->tv_nsec);
    }
  }

  if (__wasilibc_futex_wait_maybe_busy) {
    return __wasilibc_futex_wait_maybe_busy(addr, val, max_wait_ns);
  }
  return __wasilibc_futex_wait_atomic_wait(addr, val, max_wait_ns);
}

int __wasilibc_futex_wake(volatile int *addr, int count, unsigned flags)
{
	if (flags != 0 && flags != __WASILIBC_FUTEX_YIELD) {
		return -EINVAL;
	}

	if (count < 0) {
		count = INT_MAX;
	}
	return __builtin_wasm_memory_atomic_notify((int *)addr, count);
}
