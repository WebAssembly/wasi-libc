#include <termios.h>
#ifdef __wasilibc_unmodified_upstream
#include <sys/ioctl.h>
#include "syscall.h"
#else
#include <errno.h>
#include <wasi/api.h>
#include <string.h>
#endif

int tcsetwinsize(int fd, const struct winsize *wsz)
{
#ifdef __wasilibc_unmodified_upstream
	return syscall(SYS_ioctl, fd, TIOCSWINSZ, wsz);
#else
	__wasi_tty_t tty;
	int r = __wasi_tty_get(&tty);
	if (r != 0) {
		errno = r;
		return -1;
	}

	tty.cols = wsz->ws_col;
	tty.rows = wsz->ws_row;
	tty.width = wsz->ws_xpixel;
	tty.height = wsz->ws_ypixel;

	r = __wasi_tty_set(&tty);
	if (r != 0) {
		errno = r;
		return -1;
	}

	return 0;
#endif
}
