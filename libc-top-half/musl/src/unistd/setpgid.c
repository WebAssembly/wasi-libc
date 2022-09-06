#include <unistd.h>
#include <errno.h>
#ifdef __wasilibc_unmodified_upstream
#include "syscall.h"
#endif

int setpgid(pid_t pid, pid_t pgid)
{
#ifdef __wasilibc_unmodified_upstream
	return syscall(SYS_setpgid, pid, pgid);
#else
	errno = EINVAL;
	return -1;
#endif
}
