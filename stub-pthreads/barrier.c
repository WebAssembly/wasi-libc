#include "pthread_impl.h"
/*
	Note on PTHREAD_PROCESS_SHARED:
	Because WASM doesn't have virtual memory nor subprocesses,
	there isn't any way for the same synchronization object to have multiple mappings.
	Thus we can completely ignore it here.
*/

int pthread_barrier_init(pthread_barrier_t *restrict b, const pthread_barrierattr_t *restrict a, unsigned count)
{
	if (count-1 > INT_MAX-1) return EINVAL;
	*b = (pthread_barrier_t){ ._b_limit = count-1 };
	return 0;
}
int pthread_barrier_destroy(pthread_barrier_t *b)
{
	return 0;
}
int pthread_barrier_wait(pthread_barrier_t *b)
{
	if (!b->_b_limit) return PTHREAD_BARRIER_SERIAL_THREAD;
	__builtin_trap();
}
