/*
 * This file is a wrapper around malloc.c, which is the upstream source file.
 * It sets configuration flags and controls which symbols are exported.
 */

#include <stddef.h>
#include <malloc.h>

/* Define configuration macros for dlmalloc. */

/* WebAssembly doesn't have mmap-style memory allocation. */
#define HAVE_MMAP 0

/* WebAssembly doesn't support shrinking linear memory. */
#define MORECORE_CANNOT_TRIM 1

/* Disable sanity checks to reduce code size. */
#define ABORT __builtin_unreachable()

/* If threads are enabled, enable support for threads. */
#ifdef _REENTRANT
#define USE_LOCKS 1
#endif

/* Make malloc deterministic. */
#define LACKS_TIME_H 1

/* Disable malloc statistics generation to reduce code size. */
#define NO_MALLINFO 1
#define NO_MALLOC_STATS 1

/*
 * Declare errno values used by dlmalloc. We define them like this to avoid
 * putting specific errno values in the ABI.
 */
extern const int __ENOMEM;
#define ENOMEM __ENOMEM
extern const int __EINVAL;
#define EINVAL __EINVAL

/* Prefix dlmalloc's names with 'dl'. We wrap them with public names below. */
#define USE_DL_PREFIX 1
#define DLMALLOC_EXPORT static inline

/* This isn't declared with DLMALLOC_EXPORT so make it static explicitly. */
static size_t dlmalloc_usable_size(void*);

/* Include the upstream dlmalloc's malloc.c. */
#include "malloc.c"

/* Export the public names. */

void *malloc(size_t size) {
    return dlmalloc(size);
}

void free(void *ptr) {
    dlfree(ptr);
}

void *calloc(size_t nmemb, size_t size) {
    return dlcalloc(nmemb, size);
}

void *realloc(void *ptr, size_t size) {
    return dlrealloc(ptr, size);
}

int posix_memalign(void **memptr, size_t alignment, size_t size) {
    return dlposix_memalign(memptr, alignment, size);
}

void* aligned_alloc(size_t alignment, size_t bytes) {
    return dlmemalign(alignment, bytes);
}

size_t malloc_usable_size(void *ptr) {
    return dlmalloc_usable_size(ptr);
}
