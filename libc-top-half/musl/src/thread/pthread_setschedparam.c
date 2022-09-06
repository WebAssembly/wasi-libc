#include "pthread_impl.h"
#include "lock.h"

#ifdef __wasilibc_unmodified_upstream
int pthread_setschedparam(pthread_t t, int policy, const struct sched_param *param)
{
	int r;
	sigset_t set;
#ifdef __wasilibc_unmodified_upstream
	__block_app_sigs(&set);
#endif
	LOCK(t->killlock);
	r = !t->tid ? ESRCH : -__syscall(SYS_sched_setscheduler, t->tid, policy, param);
	UNLOCK(t->killlock);
#ifdef __wasilibc_unmodified_upstream
	__restore_sigs(&set);
#endif
	return r;
}
#else
int pthread_setschedparam(pthread_t t, int policy, const struct sched_param *param) {
	return EINVAL;
}
#endif