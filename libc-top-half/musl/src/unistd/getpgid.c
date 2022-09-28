#include <unistd.h>
#include <errno.h>
#ifdef __wasilibc_unmodified_upstream
#include "syscall.h"
#endif

#ifdef __wasilibc_unmodified_upstream
#else
extern int __wasilibc_pgrp;
#endif

pid_t getpgid(pid_t pid)
{
#ifdef __wasilibc_unmodified_upstream
	return syscall(SYS_getpgid, pid);
#else
	if (pid == 0) {
		return __wasilibc_pgrp;
	} else {
		errno = EINVAL;
		return -1;
	}
#endif
}
