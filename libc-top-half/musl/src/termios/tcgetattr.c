#include <termios.h>
#ifdef __wasilibc_unmodified_upstream
#include <sys/ioctl.h>
#else
#include <errno.h>
#include <wasi/api.h>
#include <string.h>
#endif

int tcgetattr(int fd, struct termios *tio)
{
#ifdef __wasilibc_unmodified_upstream	
	if (ioctl(fd, TCGETS, tio))
		return -1;
	return 0;
#else
	__wasi_tty_t tty;
	int r = __wasi_tty_get(&tty);
	if (r != 0) {
		errno = r;
		return -1;
	}

	memset(tio, 0, sizeof(struct termios));
	tio->c_lflag = ISIG|IEXTEN;
	if (tty.echo == __WASI_BOOL_TRUE) {
		tio->c_lflag |= ECHO|ECHONL;
	}
	if (tty.line_buffered == __WASI_BOOL_TRUE) {
		tio->c_lflag |= ICANON;
	}
	return 0;
#endif
}
