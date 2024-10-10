
#include "pthread_impl.h"
/* The only reason why we have to do anything is trylock */

int pthread_spin_lock(pthread_spinlock_t *s)
{
	if (*s) return EDEADLK;
	*s = 1;
	return 0;
}
int pthread_spin_trylock(pthread_spinlock_t *s)
{
	if (*s) return EBUSY;
	*s = 1;
	return 0;
}
int pthread_spin_unlock(pthread_spinlock_t *s)
{
	*s = 0;
	return 0;
}
