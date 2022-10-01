#include <termios.h>
#ifdef __wasilibc_unmodified_upstream
#include <sys/ioctl.h>
#else
#include <errno.h>
#endif

pid_t tcgetsid(int fd)
{
#ifdef __wasilibc_unmodified_upstream	
	int sid;
	if (ioctl(fd, TIOCGSID, &sid) < 0)
		return -1;
	return sid;
#else
	errno = EINVAL;
	return -1;
#endif
}
