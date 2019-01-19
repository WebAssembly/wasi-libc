#include "stdio_impl.h"
#include <sys/ioctl.h>

size_t __stdout_write(FILE *f, const unsigned char *buf, size_t len)
{
#ifdef __wasilibc_unmodified_upstream__
	struct winsize wsz;
#endif
	f->write = __stdio_write;
#ifdef __wasilibc_unmodified_upstream__
	if (!(f->flags & F_SVB) && __syscall(SYS_ioctl, f->fd, TIOCGWINSZ, &wsz))
		f->lbf = -1;
#else
	// Avoid __syscall, but also, TIOCGWINSZ is not supported in WASI.
#endif
	return __stdio_write(f, buf, len);
}
