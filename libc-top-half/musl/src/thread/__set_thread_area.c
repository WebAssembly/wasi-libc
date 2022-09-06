#include "pthread_impl.h"

int __set_thread_area(void *p)
{
#ifdef __wasilibc_unmodified_upstream
#ifdef SYS_set_thread_area
	return __syscall(SYS_set_thread_area, p);
#else
	return -ENOSYS;
#endif
#else
	return -ENOSYS;
#endif
}
