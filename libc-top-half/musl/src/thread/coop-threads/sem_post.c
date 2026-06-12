#include <semaphore.h>
#include <limits.h>
#include "pthread_impl.h"

int sem_post(sem_t *sem)
{
	int val = sem->__count;
	
	if (val == SEM_VALUE_MAX) {
		errno = EOVERFLOW;
		return -1;
	}
	
	/* Increment count */
	sem->__count = val + 1;
	
	/* If count was negative (waiters present), wake one */
	if (val < 0) {
		__waitlist_wake_one(&sem->__waiters);
	}
	
	return 0;
}