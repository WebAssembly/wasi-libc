#include "stdio_impl.h"
#include <errno.h>
#include <unistd.h>
#ifndef __wasilibc_unmodified_upstream
#include <sys/wait.h>
#endif

int pclose(FILE *f)
{
	int status, r;
	pid_t pid = f->pipe_pid;
	fclose(f);
#ifdef __wasilibc_unmodified_upstream
	while ((r=__syscall(SYS_wait4, pid, &status, 0, 0)) == -EINTR);
	if (r<0) return __syscall_ret(r);
#else
	while((r=waitpid(pid, &status, 0))<0 && errno==EINTR);
	if (r<0) return errno;
#endif
	return status;
}
