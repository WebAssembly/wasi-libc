#include <termios.h>
#ifdef __wasilibc_unmodified_upstream	
#include <sys/ioctl.h>
#endif

int tcflush(int fd, int queue)
{
#ifdef __wasilibc_unmodified_upstream	
	return ioctl(fd, TCFLSH, queue);
#else
	return 0;
#endif
}
