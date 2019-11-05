#include <errno.h>
#include <threads.h>

// These values are used by reference-sysroot's dlmalloc.
const int __EINVAL = EINVAL;
const int __ENOMEM = ENOMEM;
