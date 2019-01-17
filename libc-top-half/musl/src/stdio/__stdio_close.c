#ifdef __wasilibc_unmodified_upstream__
#else
#include <unistd.h>
#endif
#include "stdio_impl.h"

static int dummy(int fd)
{
	return fd;
}

weak_alias(dummy, __aio_close);

int __stdio_close(FILE *f)
{
#ifdef __wasilibc_unmodified_upstream__
	return syscall(SYS_close, __aio_close(f->fd));
#else
	return close(__aio_close(f->fd));
#endif
}
