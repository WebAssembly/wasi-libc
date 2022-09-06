#define _GNU_SOURCE
#include <sys/resource.h>
#include <sys/wait.h>
#ifdef __wasilibc_unmodified_upstream
#include "syscall.h"
#else
#include "pthread_impl.h"
#endif

pid_t wait3(int *status, int options, struct rusage *usage)
{
	return wait4(-1, status, options, usage);
}
