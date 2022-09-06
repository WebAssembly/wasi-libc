#include <unistd.h>
#ifdef __wasilibc_unmodified_upstream
#include "syscall.h"
#else
#include <stdlib.h>
#endif

int execve(const char *path, char *const argv[], char *const envp[])
{
#ifdef __wasilibc_unmodified_upstream
	/* do we need to use environ if envp is null? */
	return syscall(SYS_execve, path, argv, envp);
#else
	for (char **envpp = (char**)envp; *envpp != NULL; envpp++) {
		putenv(*envpp);
	}
	return execv(path, argv);
#endif
}
