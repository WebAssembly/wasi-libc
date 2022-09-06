#include <semaphore.h>
#include <sys/mman.h>
#include <unistd.h>

int sem_unlink(const char *name)
{
#ifdef __wasilibc_unmodified_upstream
	return shm_unlink(name);
#else
	return unlink(name);
#endif
}
