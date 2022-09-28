#include <unistd.h>
#include <termios.h>
#include <errno.h>
#ifdef __wasilibc_unmodified_upstream
#include <sys/ioctl.h>
#endif

#ifdef __wasilibc_unmodified_upstream
#else
extern int __wasilibc_pgrp;
#endif

pid_t tcgetpgrp(int fd)
{
#ifdef __wasilibc_unmodified_upstream
	int pgrp;
	if (ioctl(fd, TIOCGPGRP, &pgrp) < 0)
		return -1;
	return pgrp;
#else
	return __wasilibc_pgrp;
#endif
}
