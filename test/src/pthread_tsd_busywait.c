#include "src/functional/pthread_tsd.c"

#include <wasi/libc-busywait.h>

__attribute__((constructor)) void __wasilibc_enable_busywait(void) {
  __wasilibc_enable_futex_busywait_on_current_thread();
}
