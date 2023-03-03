#include <stdlib.h>
#ifdef __wasilibc_unmodified_upstream
#include <sys/wait.h>
#include "syscall.h"
#else
#include <unistd.h>
#include <wasi/api.h>
#include <errno.h>
#endif

pid_t waitpid(pid_t pid, int *status, int options)
{
#ifdef __wasilibc_unmodified_upstream
	return syscall_cp(SYS_wait4, pid, status, options, 0);
#else
	__wasi_joinflags_t flags = 0;
	if ((options & WNOHANG) != 0) {
		flags |= __WASI_JOINFLAGS_NON_BLOCKING;
	}
	if ((options & WUNTRACED) != 0) {
		flags |= __WASI_JOINFLAGS_WAKE_STOPPED;
	}

	__wasi_join_status_t code;
	int ret = __wasi_proc_join((__wasi_pid_t*)&pid, flags, &code);
	if (ret != 0) {
		errno = ret;
		return -1;
	} else {
		*status = (int)code;
		return pid;
	}
#endif
}
