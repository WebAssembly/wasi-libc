#include <sys/eventfd.h>
#include <unistd.h>
#include <errno.h>
#ifdef __wasilibc_unmodified_upstream
#include "syscall.h"
#else
#include <wasi/api.h>
#endif

int eventfd(unsigned int count, int flags)
{
#ifdef __wasilibc_unmodified_upstream
	int r = __syscall(SYS_eventfd2, count, flags);
#ifdef SYS_eventfd
	if (r==-ENOSYS && !flags) r = __syscall(SYS_eventfd, count);
#endif
	return __syscall_ret(r);
#else
	int fd = -1;
	int r = __wasi_fd_event((uint64_t)count, (uint16_t)flags, &fd);
	if (r != 0) {
		errno = r;
		return -1;
	}
	return fd;
#endif
}

int eventfd_read(int fd, eventfd_t *value)
{
	return (sizeof(*value) == read(fd, value, sizeof(*value))) ? 0 : -1;
}

int eventfd_write(int fd, eventfd_t value)
{
	return (sizeof(value) == write(fd, &value, sizeof(value))) ? 0 : -1;
}
