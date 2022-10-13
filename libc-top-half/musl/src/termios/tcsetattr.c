#include <termios.h>
#ifdef __wasilibc_unmodified_upstream
#include <sys/ioctl.h>
#else
#include <errno.h>
#include <wasi/api.h>
#include <string.h>
#endif
#include <errno.h>

int tcsetattr(int fd, int act, const struct termios *tio)
{
	if (act < 0 || act > 2) {
		errno = EINVAL;
		return -1;
	}
#ifdef __wasilibc_unmodified_upstream	
	return ioctl(fd, TCSETS+act, tio);
#else
	__wasi_tty_t tty;
	int r = __wasi_tty_get(&tty);
	if (r != 0) {
		errno = r;
		return -1;
	}

	if ((tio->c_lflag & ECHO) != 0) {
		tty.echo = __WASI_BOOL_TRUE;
	} else {
		tty.echo = __WASI_BOOL_FALSE;
	}

	if ((tio->c_lflag & ICANON) != 0) {
		tty.line_buffered = __WASI_BOOL_TRUE;
	} else {
		tty.line_buffered = __WASI_BOOL_FALSE;
	}

	r = __wasi_tty_set(&tty);
	if (r != 0) {
		errno = r;
		return -1;
	}

	return 0;
#endif
}
