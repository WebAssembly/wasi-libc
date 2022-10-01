#include <termios.h>
#include <sys/ioctl.h>
#ifdef __wasilibc_unmodified_upstream	
#include "syscall.h"
#endif

int tcdrain(int fd)
{
#ifdef __wasilibc_unmodified_upstream	
	return syscall_cp(SYS_ioctl, fd, TCSBRK, 1);
#else
	return 0;
#endif
}
