#include "pthread_impl.h"

#ifdef __wasilibc_unmodified_upstream
int pthread_getcpuclockid(pthread_t t, clockid_t *clockid)
{
	*clockid = (-t->tid-1)*8U + 6;
	return 0;
}
#else
int pthread_getcpuclockid(pthread_t t, clockid_t *clockid)
{
	return ENOENT;
}
#endif