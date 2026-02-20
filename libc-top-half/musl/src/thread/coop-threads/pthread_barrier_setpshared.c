#include "pthread_impl.h"

int pthread_barrierattr_setpshared(pthread_barrierattr_t *a, int pshared)
{
    // Only private barriers are supported
	return pshared == PTHREAD_PROCESS_PRIVATE ? 0 : ENOTSUP;
}