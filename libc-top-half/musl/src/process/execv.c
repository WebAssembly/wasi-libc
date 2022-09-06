#include <unistd.h>
#include <string.h>

#ifdef __wasilibc_unmodified_upstream
extern char **__environ;
#else
#include <stdlib.h>
#include <wasi/api.h>
#endif

int execv(const char *path, char *const argv[])
{
#ifdef __wasilibc_unmodified_upstream
	return execve(path, argv, __environ);
#else
	int combined_len = 0;
	for (char **argvp = (char **)argv; *argvp != NULL; argvp++) {
		combined_len += strlen(*argvp) + 1;
	}

	char *combined_argv = malloc((combined_len + 1));
	char *combined_argv_p = combined_argv;
	for (char **argvp = (char **)argv; *argvp != NULL; argvp++) {
		memcpy(combined_argv_p, *argvp, strlen(*argvp));
		combined_argv_p += strlen(*argvp);
		*combined_argv_p = '\n';
		combined_argv_p++;
	}
	*combined_argv_p = 0;
	
	__wasi_proc_exec(path, combined_argv);
#endif
}
