/* Stub include file to support dlmalloc. */

#include <stdint.h>

/*
 * The page size in WebAssembly is fixed at 64 KiB. If this ever changes,
 * it's expected that applications will need to opt in, so we can change
 * this.
 */
#define sysconf(name) 0x10000
#define _SC_PAGESIZE

/* Declare sbrk. */
void *sbrk(intptr_t increment);
