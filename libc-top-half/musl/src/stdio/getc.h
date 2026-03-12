#include "stdio_impl.h"
#if defined(__wasilibc_unmodified_upstream) || defined(_REENTRANT)
#include "pthread_impl.h"

#ifdef __GNUC__
__attribute__((__noinline__))
#endif
static int locking_getc(FILE *f)
{
#ifdef __wasi_cooperative_threads__
    __lockfile(f);
    int c = getc_unlocked(f);
    __unlockfile(f);
	return c;
#else
	if (a_cas(&f->lock, 0, MAYBE_WAITERS-1)) __lockfile(f);
	int c = getc_unlocked(f);
	if (a_swap(&f->lock, 0) & MAYBE_WAITERS)
		__wake(&f->lock, 1, 1);
	return c;
#endif
}
#endif

static inline int do_getc(FILE *f)
{
#ifdef __wasi_cooperative_threads__
	if (f->lock.owner == wasip3_thread_index())
		return getc_unlocked(f);
	return locking_getc(f);
#elif defined(__wasilibc_unmodified_upstream) || defined(_REENTRANT)
	int l = f->lock;
	if (l < 0 || l && (l & ~MAYBE_WAITERS) == __pthread_self()->tid)
		return getc_unlocked(f);
	return locking_getc(f);
#else
	// With no threads, locking is unnecessary.
	return getc_unlocked(f);
#endif
}
