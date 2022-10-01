#include <termios.h>
#ifdef __wasilibc_unmodified_upstream
#include <sys/ioctl.h>
#include "syscall.h"
#else
#include <errno.h>
#include <wasi/api.h>
#include <string.h>
#endif

int tcgetwinsize(int fd, struct winsize *wsz)
{
#ifdef __wasilibc_unmodified_upstream
	return syscall(SYS_ioctl, fd, TIOCGWINSZ, wsz);
#else
	__wasi_tty_t tty;
	int r = __wasi_tty_get(&tty);
	if (r != 0) {
		errno = r;
		return -1;
	}

	memset(wsz, 0, sizeof(struct termios));
	wsz->ws_col = tty.cols;
	wsz->ws_row = tty.rows;
	wsz->ws_xpixel = tty.width;
	wsz->ws_ypixel = tty.height;
	return 0;
#endif
}
