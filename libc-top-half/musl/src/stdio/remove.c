#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include "syscall.h"

int remove(const char *path)
{
#ifdef __wasilibc_unmodified_upstream__
#ifdef SYS_unlink
	int r = __syscall(SYS_unlink, path);
#else
	int r = __syscall(SYS_unlinkat, AT_FDCWD, path, 0);
#endif
#ifdef SYS_rmdir
	if (r==-EISDIR) r = __syscall(SYS_rmdir, path);
#else
	if (r==-EISDIR) r = __syscall(SYS_unlinkat, AT_FDCWD, path, AT_REMOVEDIR);
#endif
	return __syscall_ret(r);
#else
	int r = unlink(path);
	if (r != 0 && errno==EISDIR) r = rmdir(path);
	return r;
#endif
}
