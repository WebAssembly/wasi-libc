#include <semaphore.h>
#include <errno.h>

sem_t *sem_open(const char *name, int flags, ...)
{
	return ENOTSUP;
}

int sem_close(sem_t *sem)
{
	return ENOTSUP;
}
