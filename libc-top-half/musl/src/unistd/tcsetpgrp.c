#include <unistd.h>
#include <termios.h>
#include <errno.h>
#ifdef __wasilibc_unmodified_upstream
#include <sys/ioctl.h>
#endif

int tcsetpgrp(int fd, pid_t pgrp)
{
#ifdef __wasilibc_unmodified_upstream
	int pgrp_int = pgrp;
	return ioctl(fd, TIOCSPGRP, &pgrp_int);
#else
	errno = EINVAL;
	return -1;
#endif
}
