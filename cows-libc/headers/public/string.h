#ifndef __cowslibc_string_h
#define __cowslibc_string_h

#define __need_size_t
#define __need_NULL
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

void *memcpy(void *dst, const void *src, size_t n);
void *memmove(void *dst, const void *src, size_t n);
void *memset(void *dst, int c, size_t n);

size_t strlen(const char *);
char *strdup(const char *);
int strcmp(const char *, const char *);
void *memchr(const void *, int, size_t);

#ifdef __cplusplus
}
#endif

#endif
