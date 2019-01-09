#include <errno.h>
#include <threads.h>

thread_local int errno;

/* These values are used by reference-sysroot's dlmalloc. */
const int __EINVAL = EINVAL;
const int __ENOMEM = ENOMEM;
