#include "pthread_impl.h"
#include <threads.h>

#if !defined(__wasilibc_unmodified_upstream) && defined(__wasm__) &&           \
    defined(_REENTRANT)
// We need some place to store the thread ID. This WebAssembly thread_local fits the
// bill and is used by `__get_tp` elsewhere.
thread_local uintptr_t __wasilibc_pthread_self = 0;
#endif

static pthread_t __pthread_self_internal()
{
	return __pthread_self();
}

weak_alias(__pthread_self_internal, pthread_self);
weak_alias(__pthread_self_internal, thrd_current);
