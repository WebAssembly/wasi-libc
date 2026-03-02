#include <pthread.h>

#if defined(_REENTRANT) && !defined(__wasi_cooperative_threads__)
static pthread_rwlock_t lock = PTHREAD_RWLOCK_INITIALIZER;

void __inhibit_ptc()
{
	pthread_rwlock_wrlock(&lock);
}

void __acquire_ptc()
{
	pthread_rwlock_rdlock(&lock);
}

void __release_ptc()
{
	pthread_rwlock_unlock(&lock);
}
#else
void __inhibit_ptc(){}
void __acquire_ptc(){}
void __release_ptc(){}
#endif