#include "pthread_impl.h"

int pthread_getcpuclockid(pthread_t t, clockid_t *clockid)
{
#ifdef __wasilibc_unmodified_upstream
	*clockid = (-t->tid-1)*8U + 6;
	return 0;
#else
        return ENOTSUP;
#endif
}
