#include <termios.h>
#include <sys/ioctl.h>
#include "syscall.h"

int tcdrain(int fd)
{
#ifdef __wasilibc_unmodified_upstream__
	return syscall_cp(SYS_ioctl, fd, TCSBRK, 1);
#else
	return ioctl(fd, TCSBRK, 1);
#endif
}
