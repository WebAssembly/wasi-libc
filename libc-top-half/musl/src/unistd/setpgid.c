#include <unistd.h>
#include <errno.h>
#ifdef __wasilibc_unmodified_upstream
#include "syscall.h"
#endif

#ifdef __wasilibc_unmodified_upstream
#else
extern int __wasilibc_pgrp;
#endif

int setpgid(pid_t pid, pid_t pgid)
{
#ifdef __wasilibc_unmodified_upstream
	return syscall(SYS_setpgid, pid, pgid);
#else
	if (pid == 0) {
		__wasilibc_pgrp = pgid;
		return 0;
	} else {
		errno = EINVAL;
		return -1;
	}
#endif
}
