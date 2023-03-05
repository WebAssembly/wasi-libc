#include "pthread_impl.h"
#include <threads.h>

#if !defined(__wasilibc_unmodified_upstream) && defined(__wasm__) &&           \
    defined(_REENTRANT)
// We need some place to store the thread ID. This WebAssembly global fits the
// bill and is used by `__get_tp` elsewhere.
#if defined(__wasm64__)
__asm__(".globaltype __wasilibc_pthread_self, i64\n");
#else
__asm__(".globaltype __wasilibc_pthread_self, i32\n");
#endif
#endif

static pthread_t __pthread_self_internal()
{
	return __pthread_self();
}

weak_alias(__pthread_self_internal, pthread_self);
weak_alias(__pthread_self_internal, thrd_current);
