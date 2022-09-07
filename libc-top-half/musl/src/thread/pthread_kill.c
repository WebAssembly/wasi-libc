#include "pthread_impl.h"
#include "lock.h"
#ifdef __wasilibc_unmodified_upstream
#else
#include <wasi/api.h>
#include "signal.h"
#endif

#ifdef __wasilibc_unmodified_upstream
int pthread_kill(pthread_t t, int sig)
{
	int r;
	sigset_t set;
	/* Block not just app signals, but internal ones too, since
	 * pthread_kill is used to implement pthread_cancel, which
	 * must be async-cancel-safe. */
	__block_all_sigs(&set);
	LOCK(t->killlock);
	r = t->tid ? -__syscall(SYS_tkill, t->tid, sig)
		: (sig+0U >= _NSIG ? EINVAL : 0);
	UNLOCK(t->killlock);
	__restore_sigs(&set);
	return r;
}
#else
int pthread_kill(pthread_t t, int sig)
{
	sigset_t set;
	__block_all_sigs(&set);
	int r = __wasi_thread_signal(t->tid, (__wasi_signal_t)sig);
	__restore_sigs(&set);
	return r;
}
#endif