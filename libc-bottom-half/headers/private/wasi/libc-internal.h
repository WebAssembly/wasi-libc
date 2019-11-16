#ifndef __wasi_libc_internal_h
#define __wasi_libc_internal_h

#include <wasi/api.h>

#ifdef __cplusplus
extern "C" {
#endif

void __wasilibc_init_preopen(void);
__wasi_errno_t __wasilibc_populate_environ(void);

#ifdef __cplusplus
}
#endif

#endif
