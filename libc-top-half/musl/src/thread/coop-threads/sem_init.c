#include <semaphore.h>
#include <limits.h>
#include <errno.h>
#include <stdlib.h>

int sem_init(sem_t *sem, int pshared, unsigned value)
{
	if (value > SEM_VALUE_MAX) {
		errno = EINVAL;
		return -1;
	}
	sem->__count = value;
	sem->__waiters = NULL;
	return 0;
}
