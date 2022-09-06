#include <wasi/api.h>
#include <sys/types.h>
#include <stdlib.h>

void __wasilibc_futex_wait(int* futex, int expected) {
  __wasi_bool_t woken = __WASI_BOOL_FALSE;
  
  __wasi_option_timestamp_t timeout;
  timeout.tag = __WASI_OPTION_NONE;
  timeout.u.none = 0;

  for (;woken == __WASI_BOOL_FALSE;) {
    if (__wasi_futex_wait((uint32_t*)futex, expected, &timeout, &woken) != 0) {
      __builtin_trap();
    }
  }
}

void __wasilibc_futex_wake(int* futex, int cnt) {
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
}
