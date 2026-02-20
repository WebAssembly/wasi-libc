#include "pthread_impl.h"

int __pthread_cond_timedwait(pthread_cond_t *restrict c, pthread_mutex_t *restrict m, const struct timespec *restrict ts)
{
	// TODO(wasip3) timed waits
	if (ts) {
		return ETIMEDOUT;
	}
	
	pthread_mutex_unlock(m);
	__waitlist_wait_on(&c->_c_waiters);
	return pthread_mutex_lock(m);
}