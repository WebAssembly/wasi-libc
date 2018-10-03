#ifndef __wasm_sysroot_stdlib_h
#define __wasm_sysroot_stdlib_h

#define __need_size_t
#define __need_wchar_t
#define __need_NULL
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

void *malloc(size_t size);
void free(void *ptr);
void *calloc(size_t nmemb, size_t size);
void *realloc(void *ptr, size_t size);

#ifdef __cplusplus
}
#endif

#endif
