#include <unistd.h>
#include <string.h>
#include <stdio.h>

#ifdef __wasilibc_unmodified_upstream
extern char **__environ;
#else
#include <stdlib.h>
#include <wasi/api.h>
#endif

int execve(const char *path, char *const argv[], char *const envp[])
{
#ifdef __wasilibc_unmodified_upstream
	return execve(path, argv, __environ);
#else
	// combine args
	int argv_combined_len = 0;
	for (char **argvp = (char **)argv; *argvp != NULL; argvp++) {
		argv_combined_len += strlen(*argvp) + 1;
	}

	char *combined_argv = malloc((argv_combined_len + 1));
	char *combined_argv_p = combined_argv;
	for (char **argvp = (char **)argv; *argvp != NULL; argvp++) {
		memcpy(combined_argv_p, *argvp, strlen(*argvp));
		combined_argv_p += strlen(*argvp);
		*combined_argv_p = '\n';
		combined_argv_p++;
	}
	*combined_argv_p = 0;

	// combine env vars
	int env_combined_len = 0;
	for (char **envpp = (char **)envp; *envpp != NULL; envpp++) {
		env_combined_len += strlen(*envpp) + 1;
	}

	char *combined_env = malloc((env_combined_len + 1));
	char *combined_env_p = combined_env;
	for (char **envpp = (char **)envp; *envpp != NULL; envpp++) {
		memcpy(combined_env_p, *envpp, strlen(*envpp));
		combined_env_p += strlen(*envpp);
		*combined_env_p = '\n';
		combined_env_p++;
	}
	*combined_env_p = 0;

	printf("calling the __wasi_proc_exec2\n");
	
	__wasi_proc_exec2(path, combined_argv, combined_env);
#endif
}
