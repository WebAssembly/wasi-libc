#include <threads.h>

static inline uintptr_t __get_tp(void) {
#if _REENTRANT
	extern thread_local uintptr_t __wasilibc_pthread_self;
	return __wasilibc_pthread_self;
#else
	return 0;
#endif
}

