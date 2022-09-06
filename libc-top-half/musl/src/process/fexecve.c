#define _GNU_SOURCE
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#ifdef __wasilibc_unmodified_upstream
#include "syscall.h"
#endif

int fexecve(int fd, char *const argv[], char *const envp[])
{
#ifdef __wasilibc_unmodified_upstream
	int r = __syscall(SYS_execveat, fd, "", argv, envp, AT_EMPTY_PATH);
	if (r != -ENOSYS) return __syscall_ret(r);
	char buf[15 + 3*sizeof(int)];
	__procfdname(buf, fd);
	execve(buf, argv, envp);
	if (errno == ENOENT) errno = EBADF;
	return -1;
#else
	errno = EINVAL;
	return -1;
#endif
}
