#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <limits.h>

extern char **__wasilibc_environ;

#ifdef __wasilibc_unmodified_upstream
int __execvpe(const char *file, char *const argv[], char *const envp[])
{
	const char *p, *z, *path = getenv("PATH");
	size_t l, k;
	int seen_eacces = 0;

	errno = ENOENT;
	if (!*file)
		return -1;

	if (strchr(file, '/'))
		return execve(file, argv, envp);

	if (!path)
		path = "/usr/local/bin:/bin:/usr/bin";
	k = strnlen(file, NAME_MAX + 1);
	if (k > NAME_MAX)
	{
		errno = ENAMETOOLONG;
		return -1;
	}
	l = strnlen(path, PATH_MAX - 1) + 1;

	for (p = path;; p = z)
	{
		char b[l + k + 1];
		z = __strchrnul(p, ':');
		if (z - p >= l)
		{
			if (!*z++)
				break;
			continue;
		}
		memcpy(b, p, z - p);
		b[z - p] = '/';
		memcpy(b + (z - p) + (z > p), file, k + 1);
		execve(b, argv, envp);
		switch (errno)
		{
		case EACCES:
			seen_eacces = 1;
		case ENOENT:
		case ENOTDIR:
			break;
		default:
			return -1;
		}
		if (!*z++)
			break;
	}
	if (seen_eacces)
		errno = EACCES;
	return -1;
}
#else
char *__wasilibc_exec_combine_strings(char *const strings[])
{
	int combined_len = 0;
	for (char **ptr = (char **)strings; *ptr != NULL; ptr++)
	{
		combined_len += strlen(*ptr) + 1;
	}

	char *combined = malloc((combined_len + 1));
	char *combined_p = combined;
	for (char **ptr = (char **)strings; *ptr != NULL; ptr++)
	{
		memcpy(combined_p, *ptr, strlen(*ptr));
		combined_p += strlen(*ptr);
		*combined_p = '\n';
		combined_p++;
	}
	*combined_p = 0;

	return combined;
}

int __execvpe(const char *path, char *const argv[], char *const envp[], uint8_t use_path)
{
	char *combined_argv = __wasilibc_exec_combine_strings(argv);
	char *combined_env = __wasilibc_exec_combine_strings(envp);

	int e = __wasi_proc_exec3(
		path, combined_argv, combined_env,
		use_path ? __WASI_BOOL_TRUE : __WASI_BOOL_FALSE, getenv("PATH"));

	free(combined_argv);
	free(combined_env);

	// A return from proc_exec automatically means it failed
	errno = e;
	return -1;
}
#endif

int __execvp(const char *file, char *const argv[])
{
#ifndef __wasilibc_unmodified_upstream
	__wasilibc_ensure_environ();
#endif
	return __execvpe(file, argv, __wasilibc_environ, 1);
}

weak_alias(__execvp, execvp);
weak_alias(__execvp, execvpe);
