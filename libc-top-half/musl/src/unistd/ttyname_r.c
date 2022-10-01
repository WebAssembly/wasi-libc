#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#ifdef __wasilibc_unmodified_upstream /* WASI has no ttyname */
#include "syscall.h"
#else
#include <wasi/api.h>
#include <string.h>
#endif

int ttyname_r(int fd, char *name, size_t size)
{
#ifdef __wasilibc_unmodified_upstream /* WASI has no ttyname */
	struct stat st1, st2;
	char procname[sizeof "/proc/self/fd/" + 3*sizeof(int) + 2];
	ssize_t l;

	if (!isatty(fd)) return errno;

	__procfdname(procname, fd);
	l = readlink(procname, name, size);

	if (l < 0) return errno;
	else if (l == size) return ERANGE;

	name[l] = 0;

	if (stat(name, &st1) || fstat(fd, &st2))
		return errno;
	if (st1.st_dev != st2.st_dev || st1.st_ino != st2.st_ino)
		return ENODEV;

	return 0;
#else
	__wasi_tty_t tty;
	int r = __wasi_tty_get(&tty);
	if (r != 0) {
		errno = r;
		return 0;
	}
	if (fd == 0 && tty.stdin_tty == __WASI_BOOL_TRUE)  {
		strncpy(name, "/dev/stdin", size);
		return 0;
	}
	if (fd == 1 && tty.stdout_tty == __WASI_BOOL_TRUE)  {
		strncpy(name, "/dev/stdout", size);
		return 0;
	}
	if (fd == 2 && tty.stderr_tty == __WASI_BOOL_TRUE)  {
		strncpy(name, "/dev/stderr", size);
		return 0;
	}
	return ENOTTY;
#endif
}
