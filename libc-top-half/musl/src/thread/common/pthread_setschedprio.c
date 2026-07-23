#include "pthread_impl.h"
#include "lock.h"

int pthread_setschedprio(pthread_t t, int prio)
{
#ifdef __wasilibc_unmodified_upstream
	int r;
	sigset_t set;
	__block_app_sigs(&set);
	LOCK(t->killlock);
	r = !t->tid ? ESRCH : -__syscall(SYS_sched_setparam, t->tid, &prio);
	UNLOCK(t->killlock);
	__restore_sigs(&set);
	return r;
#else
        return ENOTSUP;
#endif
}
