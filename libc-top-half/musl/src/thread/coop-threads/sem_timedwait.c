#include <semaphore.h>
#include <limits.h>
#include "pthread_impl.h"

int sem_timedwait(sem_t *restrict sem, const struct timespec *restrict at)
{
	pthread_testcancel();
	
	/* Fast path: if permit is available, take it regardless of timeout */
	if (sem->__count > 0) {
		sem->__count--;
		return 0;
	}
	
	/* Would need to block, but timeouts not supported in cooperative threading.
	 * There's no way to interrupt a suspended thread after a timeout. */
	if (at) {
		errno = ENOSYS;
		return -1;
	}
	
	/* Decrement count to indicate we're waiting (goes negative) */
	sem->__count--;
	
	/* Loop until a permit becomes available */
	while (sem->__count < 0) {
		/* No permits available, wait on the waitlist */
		__waitlist_wait_on(&sem->__waiters);
		/* After waking, recheck - another thread might have taken the permit */
	}
	
	/* We've been given a permit (count incremented by sem_post) */
	return 0;
}