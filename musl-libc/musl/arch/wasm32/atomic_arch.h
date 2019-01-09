#ifndef __WASM_THREAD_MODEL_SINGLE
#error "multiple threads not supported in musl yet"
#endif

#include <stdlib.h>

#define a_barrier() ((void)0)

#define a_cas a_cas
static inline int a_cas(volatile int *p, int t, int s)
{
  int old = *p;
  if (old == t)
    *p = s;
  return old;
}

#define a_crash() abort()
