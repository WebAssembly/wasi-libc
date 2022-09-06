#include "pthread_impl.h"
#include "lock.h"

#ifdef __wasilibc_unmodified_upstream
int pthread_setschedprio(pthread_t t, int prio)
{
	int r;
	sigset_t set;
#ifdef __wasilibc_unmodified_upstream
	__block_app_sigs(&set);
#endif
	LOCK(t->killlock);
	r = !t->tid ? ESRCH : -__syscall(SYS_sched_setparam, t->tid, &prio);
	UNLOCK(t->killlock);
#ifdef __wasilibc_unmodified_upstream
	__restore_sigs(&set);
#endif
	return r;
}
#else
int pthread_setschedprio(pthread_t t, int prio)
{
	return EINVAL;
}
#endif