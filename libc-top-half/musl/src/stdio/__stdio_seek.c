#ifdef __wasilibc_unmodified_upstream // WASI has no syscall
#else
#include <unistd.h>
#endif
#include "stdio_impl.h"
#include "stdio_impl.h"

off_t __stdio_seek(FILE *f, off_t off, int whence)
{
	off_t ret;
#ifdef SYS__llseek
#ifdef __wasilibc_unmodified_upstream // WASI has no syscall
	if (syscall(SYS__llseek, f->fd, off>>32, off, &ret, whence)<0)
#else
	if (llseek(f->fd, off>>32, off, &ret, whence)<0)
#endif
		ret = -1;
#else
#ifdef __wasilibc_unmodified_upstream // WASI has no syscall
	ret = syscall(SYS_lseek, f->fd, off, whence);
#else
	ret = lseek(f->fd, off, whence);
#endif
#endif
	return ret;
}
