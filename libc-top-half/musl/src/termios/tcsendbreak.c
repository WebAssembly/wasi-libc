#include <termios.h>
#ifdef __wasilibc_unmodified_upstream
#include <sys/ioctl.h>
#endif

int tcsendbreak(int fd, int dur)
{
#ifdef __wasilibc_unmodified_upstream
	/* nonzero duration is implementation-defined, so ignore it */
	return ioctl(fd, TCSBRK, 0);
#else
	return 0;
#endif
}
