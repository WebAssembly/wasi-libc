#include <unistd.h>
#include <string.h>
#include <stdio.h>

#ifdef __wasilibc_unmodified_upstream
#include "syscall.h"
#else
#include <stdlib.h>
#include <wasi/api.h>
#endif

#if !defined(__wasilibc_unmodified_upstream)
static char* combine_strings(char* const strings[]) {
	int combined_len = 0;
	for (char **ptr = (char **)strings; *ptr != NULL; ptr++) {
		combined_len += strlen(*ptr) + 1;
	}

	char *combined = malloc((combined_len + 1));
	char *combined_p = combined;
	for (char **ptr = (char **)strings; *ptr != NULL; ptr++) {
		memcpy(combined_p, *ptr, strlen(*ptr));
		combined_p += strlen(*ptr);
		*combined_p = '\n';
		combined_p++;
	}
	*combined_p = 0;

	return combined;
}
#endif

int execve(const char *path, char *const argv[], char *const envp[])
{
#ifdef __wasilibc_unmodified_upstream	
	/* do we need to use environ if envp is null? */
	return syscall(SYS_execve, path, argv, envp);
#else	
	char *combined_argv = combine_strings(argv);
	char *combined_env = combine_strings(envp);

	printf("calling the __wasi_proc_exec2\n");
	
	__wasi_proc_exec2(path, combined_argv, combined_env);
#endif
}
