#include <wasi/api.h>
#include <sys/types.h>
#include <limits.h>
#include <stdlib.h>

int __wasilibc_futex_wait_wasix(volatile void *futex, int op, int expected, int64_t max_wait_ns) {
  __wasi_bool_t woken = __WASI_BOOL_FALSE;
  
  __wasi_option_timestamp_t timeout;
  if (max_wait_ns > 0) {
    timeout.tag = __WASI_OPTION_SOME;
    timeout.u.none = max_wait_ns;
  } else {
    timeout.tag = __WASI_OPTION_NONE;
    timeout.u.none = 0;
  }

  for (;woken == __WASI_BOOL_FALSE;) {
    if (__wasi_futex_wait((uint32_t*)futex, expected, &timeout, &woken) != 0) {
      __builtin_trap();
    }
  }
  return 0;
}

int __wasilibc_futex_wake_wasix(int* futex, int cnt) {
  __wasi_bool_t woken = __WASI_BOOL_FALSE;
  for (;woken == __WASI_BOOL_FALSE;) {
    if (cnt == INT_MAX) {
      for (int n = 0; n < cnt; n++) {
        if (__wasi_futex_wake((uint32_t*)futex, &woken) != 0) {
          __builtin_trap();
        }
      }
    } else {
      if (__wasi_futex_wake_all((uint32_t*)futex, &woken) != 0) {
        __builtin_trap();
      }
    }
  }
  return 0;
}
