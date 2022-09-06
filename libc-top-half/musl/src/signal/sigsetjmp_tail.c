#include <setjmp.h>
#include <signal.h>
#include <errno.h>
#ifdef __wasilibc_unmodified_upstream
#include "syscall.h"
#endif

hidden int __sigsetjmp_tail(sigjmp_buf jb, int ret)
{
#ifdef __wasilibc_unmodified_upstream
	void *p = jb->__ss;
	__syscall(SYS_rt_sigprocmask, SIG_SETMASK, ret?p:0, ret?0:p, _NSIG/8);
	return ret;
#else
	return EINVAL;
#endif
}