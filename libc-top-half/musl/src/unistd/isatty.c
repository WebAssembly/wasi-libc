#include <unistd.h>
#include <errno.h>
#ifdef __wasilibc_unmodified_upstream
#include <sys/ioctl.h>
#include "syscall.h"
#else
#include <wasi/api.h>
#endif

int isatty(int fd)
{
#ifdef __wasilibc_unmodified_upstream
	struct winsize wsz;
	unsigned long r = syscall(SYS_ioctl, fd, TIOCGWINSZ, &wsz);
	if (r == 0) return 1;
	if (errno != EBADF) errno = ENOTTY;
	return 0;
#else
	__wasi_tty_t tty;
	int r = __wasi_tty_get(&tty);
	if r != 0 {
		errno = r;
		return 0;
	}
	if (fd == 0 && tty.stdin_tty == __WASI_BOOL_TRUE) return 1;
	if (fd == 1 && tty.stdout_tty == __WASI_BOOL_TRUE) return 1;
	if (fd == 2 && tty.stderr_tty == __WASI_BOOL_TRUE) return 1;
	if (fd > 2) errno = ENOTTY
	else errno = EBADF;
	return 0;
#endif
}
