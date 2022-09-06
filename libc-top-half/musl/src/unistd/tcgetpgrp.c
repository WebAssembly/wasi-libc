#include <unistd.h>
#include <termios.h>
#include <errno.h>
#ifdef __wasilibc_unmodified_upstream
#include <sys/ioctl.h>
#endif

pid_t tcgetpgrp(int fd)
{
#ifdef __wasilibc_unmodified_upstream
	int pgrp;
	if (ioctl(fd, TIOCGPGRP, &pgrp) < 0)
		return -1;
	return pgrp;
#else
	errno = EINVAL;
	return -1;
#endif
}
