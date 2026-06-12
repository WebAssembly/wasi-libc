#include <semaphore.h>
#include <errno.h>
#include <stdlib.h>

sem_t *sem_open(const char *name, int flags, ...)
{
	return NULL;
}

int sem_close(sem_t *sem)
{
	return ENOTSUP;
}
