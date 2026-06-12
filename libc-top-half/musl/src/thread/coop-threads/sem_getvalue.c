#include <semaphore.h>

int sem_getvalue(sem_t *restrict sem, int *restrict valp)
{
	*valp = sem->__count;
	return 0;
}
