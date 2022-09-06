#define _GNU_SOURCE
#include <stdlib.h>
#include "libc.h"

char *secure_getenv(const char *name)
{
#ifdef __wasilibc_unmodified_upstream
	return libc.secure ? NULL : getenv(name);
#else
    return getenv(name);
#endif
}
