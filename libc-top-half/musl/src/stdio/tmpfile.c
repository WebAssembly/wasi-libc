#ifdef __wasilibc_unmodified_upstream
#else
#include <unistd.h>
#include <wasi/libc.h>
#endif
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include "stdio_impl.h"

#define MAXTRIES 100

FILE *tmpfile(void)
{
	char s[] = "/tmp/tmpfile_XXXXXX";
	int fd;
	FILE *f;
	int try;
	for (try=0; try<MAXTRIES; try++) {
		__randname(s+13);
#ifdef __wasilibc_unmodified_upstream
		fd = sys_open(s, O_RDWR|O_CREAT|O_EXCL, 0600);
		if (fd >= 0) {
#ifdef SYS_unlink
			__syscall(SYS_unlink, s);
#else
			__syscall(SYS_unlinkat, AT_FDCWD, s, 0);
#endif
			f = __fdopen(fd, "w+");
			if (!f) __syscall(SYS_close, fd);
			return f;
		}
#else
		// just create a new one
		fd = __wasilibc_open_nomode(s, O_RDWR|O_CREAT|O_EXCL);
		if(fd >= 0) {
			f = __fdopen(fd, "w+");
			if (!f) close(fd);
			return f;
		}
#endif
	}
	return 0;
}

#ifdef __wasilibc_unmodified_upstream
weak_alias(tmpfile, tmpfile64);
#endif