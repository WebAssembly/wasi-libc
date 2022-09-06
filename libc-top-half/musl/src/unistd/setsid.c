#include <unistd.h>
#include <errno.h>
#ifdef __wasilibc_unmodified_upstream
#include "syscall.h"
#endif

pid_t setsid(void)
{
#ifdef __wasilibc_unmodified_upstream
	return syscall(SYS_setsid);
#else
	errno = EINVAL;
	return -1;
#endif
}
