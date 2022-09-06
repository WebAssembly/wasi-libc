#include <unistd.h>
#include <errno.h>
#ifdef __wasilibc_unmodified_upstream
#include "syscall.h"
#endif

pid_t getpgrp(void)
{
#ifdef __wasilibc_unmodified_upstream
	return __syscall(SYS_getpgid, 0);
#else
	errno = EINVAL;
	return -1;
#endif
}
