#include <signal.h>
#include <errno.h>
#ifdef __wasilibc_unmodified_upstream
#include "syscall.h"
#endif

int sigsuspend(const sigset_t *mask)
{
#ifdef __wasilibc_unmodified_upstream
	return syscall_cp(SYS_rt_sigsuspend, mask, _NSIG/8);
#else
	return EINVAL;
#endif
}