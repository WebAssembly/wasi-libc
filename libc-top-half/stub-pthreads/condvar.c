#include "pthread_impl.h"
#include <time.h>

int pthread_cond_init(pthread_cond_t *restrict c, const pthread_condattr_t *restrict a)
{
	return 0;
}
int pthread_cond_destroy(pthread_cond_t *c)
{
	return 0;
}
int pthread_cond_broadcast(pthread_cond_t *c)
{
	return 0;
}
int pthread_cond_signal(pthread_cond_t *c)
{
	return 0;
}
int pthread_cond_wait(pthread_cond_t *restrict c, pthread_mutex_t *restrict m)
{
	/* Because there is no other thread that can signal us, this is a deadlock immediately.
	The other possible choice is to return immediately (spurious wakeup), but that is likely to
	just result in the program spinning forever on a condition that cannot become true. */
	__builtin_trap();
}
int __pthread_cond_timedwait(pthread_cond_t *restrict c, pthread_mutex_t *restrict m, const struct timespec *restrict ts)
{
	/* Error check mutexes must detect if they're not locked (UB for others) */
	if (!m->_m_count) return EPERM;
	int ret = clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, ts, 0);
	if (ret == 0) return ETIMEDOUT;
	if (ret != EINTR) return ret;
	return 0;
}
weak_alias(__pthread_cond_timedwait, pthread_cond_timedwait);
