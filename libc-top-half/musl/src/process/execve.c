#include <unistd.h>
#include <string.h>
#include <stdio.h>

#ifdef __wasilibc_unmodified_upstream
#include "syscall.h"
#else
#include <stdlib.h>
#include <errno.h>
#include <wasi/api.h>
#endif

int execve(const char *path, char *const argv[], char *const envp[])
{
#ifdef __wasilibc_unmodified_upstream	
	/* do we need to use environ if envp is null? */
	return syscall(SYS_execve, path, argv, envp);
#else	
	return __execvpe(path, argv, envp, 0);
#endif
}
