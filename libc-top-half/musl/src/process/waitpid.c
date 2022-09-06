#ifdef __wasilibc_unmodified_upstream
#include <sys/wait.h>
#include "syscall.h"
#else
#include <unistd.h>
#include <wasi/api.h>
#endif

pid_t waitpid(pid_t pid, int *status, int options)
{
#ifdef __wasilibc_unmodified_upstream
	return syscall_cp(SYS_wait4, pid, status, options, 0);
#else
	__wasi_exitcode_t code;
	return __wasi_proc_join((__wasi_pid_t)pid, &code);
#endif
}
