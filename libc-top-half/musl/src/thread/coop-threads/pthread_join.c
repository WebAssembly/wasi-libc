#define _GNU_SOURCE
#include "pthread_impl.h"

static int __pthread_timedjoin_np(pthread_t t, void **res, const struct timespec *at)
{
	int state = t->detach_state;
	
	/* Cannot join a detached thread */
	if (state >= DT_DETACHED) {
		return EINVAL;
	}
	
	/* If already exited, just return the result */
	if (state == DT_EXITED) {
		if (res) *res = t->result;
		if (t->map_base) free(t->map_base);
		return 0;
	}
	
	/* Timeouts not supported for cooperative threading */
	if (at) {
		return ETIMEDOUT;
	}
	
    if (t->joiner_waiters) {
        // Only one thread can wait to join at a time
        return EINVAL;
    }

	__waitlist_wait_on(&t->joiner_waiters);

	if (res) *res = t->result;
	if (t->map_base) free(t->map_base);
	return 0;
}

int __pthread_join(pthread_t t, void **res)
{
	return __pthread_timedjoin_np(t, res, 0);
}

static int __pthread_tryjoin_np(pthread_t t, void **res)
{
	return t->detach_state==DT_JOINABLE ? EBUSY : __pthread_join(t, res);
}

weak_alias(__pthread_tryjoin_np, pthread_tryjoin_np);
weak_alias(__pthread_timedjoin_np, pthread_timedjoin_np);
weak_alias(__pthread_join, pthread_join);
