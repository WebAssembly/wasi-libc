#include "pthread_impl.h"

int pthread_spin_lock(pthread_spinlock_t *s)
{
	if (*s) return EDEADLK;
	*s = EBUSY;
	return 0;
}
