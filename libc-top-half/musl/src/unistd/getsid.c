#include <unistd.h>
#include <errno.h>
#ifdef __wasilibc_unmodified_upstream
#include "syscall.h"
#endif

pid_t getsid(pid_t pid)
{
#ifdef __wasilibc_unmodified_upstream
	return syscall(SYS_getsid, pid);
#else
	errno = EINVAL;
	return -1;
#endif
}
