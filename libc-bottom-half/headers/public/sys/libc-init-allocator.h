#ifndef __wasi_libc_init_allocator_h
#define __wasi_libc_init_allocator_h

#include <wasi/core.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Initialize the initial state of WASI allocator to use free memory between __heap_base and initial */
void __wasilibc_try_init_allocator(void);

#ifdef __cplusplus
}
#endif

#endif
