#include <threads.h>
#ifdef __wasilibc_unmodified_upstream
#include "syscall.h"
#else
#include <wasi/api.h>
#endif

void thrd_yield()
{
#ifdef __wasilibc_unmodified_upstream
  __syscall(SYS_sched_yield);
#else
  uint16_t ret = __wasi_sched_yield();
#endif
}
