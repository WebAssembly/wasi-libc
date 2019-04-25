#include "stdio_impl.h"

static size_t do_read(FILE *f, unsigned char *buf, size_t len)
{
	return __string_read(f, buf, len);
}

int vsscanf(const char *restrict s, const char *restrict fmt, va_list ap)
{
	FILE f = {
		.buf = (void *)s, .cookie = (void *)s,
#if defined(__wasilibc_unmodified_upstream) || defined(_REENTRANT)
		.read = do_read, .lock = -1
#else
		.read = do_read
#endif
	};
	return vfscanf(&f, fmt, ap);
}

weak_alias(vsscanf,__isoc99_vsscanf);
