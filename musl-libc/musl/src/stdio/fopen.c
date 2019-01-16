#ifdef __wasm_musl_unmodified_upstream__
#else
#include <unistd.h>
#endif
#include "stdio_impl.h"
#include <fcntl.h>
#include <string.h>
#include <errno.h>

FILE *fopen(const char *restrict filename, const char *restrict mode)
{
	FILE *f;
	int fd;
	int flags;

	/* Check for valid initial mode character */
	if (!strchr("rwa", *mode)) {
		errno = EINVAL;
		return 0;
	}

	/* Compute the flags to pass to open() */
	flags = __fmodeflags(mode);

#ifdef __wasm_musl_unmodified_upstream__
	fd = sys_open(filename, flags, 0666);
#else
	fd = open(filename, flags, 0666);
#endif
	if (fd < 0) return 0;
	if (flags & O_CLOEXEC)
#ifdef __wasm_musl_unmodified_upstream__
		__syscall(SYS_fcntl, fd, F_SETFD, FD_CLOEXEC);
#else
		// Avoid __syscall, but also, FD_CLOEXEC is not supported in WASI.
#endif

	f = __fdopen(fd, mode);
	if (f) return f;

#ifdef __wasm_musl_unmodified_upstream__
	__syscall(SYS_close, fd);
#else
        close(fd);
#endif
	return 0;
}

weak_alias(fopen, fopen64);
