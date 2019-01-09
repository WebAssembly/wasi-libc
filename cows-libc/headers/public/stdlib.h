#ifndef __cowslibc_stdlib_h
#define __cowslibc_stdlib_h

#define __need_size_t
#include <stddef.h>

void abort(void) __attribute__ ((__noreturn__));
void qsort(void *, size_t, size_t,
           int (*)(const void *, const void *));

#endif
