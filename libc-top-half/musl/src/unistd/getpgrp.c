#include <unistd.h>
#include <errno.h>
#ifdef __wasilibc_unmodified_upstream
#include "syscall.h"
#endif

#ifdef __wasilibc_unmodified_upstream
#else
int __wasilibc_pgrp = 0;
#endif

pid_t getpgrp(void)
{
#ifdef __wasilibc_unmodified_upstream
	return __syscall(SYS_getpgid, 0);
#else
	return __wasilibc_pgrp;
#endif
}
