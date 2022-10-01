#include <termios.h>
#ifdef __wasilibc_unmodified_upstream	
#include <sys/ioctl.h>
#endif

int tcflow(int fd, int action)
{
#ifdef __wasilibc_unmodified_upstream	
	return ioctl(fd, TCXONC, action);
#else
	return 0;
#endif
}
