#include <string.h>
#include <stdint.h>

static void *copy_forward(void *restrict dst, const void *restrict src, size_t n) {
    char *d = (char *)dst;
    const char *s = (const char *)src;
    while (n-- != 0) {
        *d++ = *s++;
    }
    return dst;
}

static void *copy_backward(void *restrict dst, const void *restrict src, size_t n) {
    char *d = (char *)dst;
    const char *s = (const char *)src;
    d += n;
    s += n;
    while (n-- != 0) {
        *--d = *--s;
    }
    return dst;
}

void *memcpy(void *restrict dst, const void *restrict src, size_t n) {
    return copy_forward(dst, src, n);
}

void *memmove(void *dst, const void *src, size_t n) {
    if ((uintptr_t)dst - (uintptr_t)src >= n) {
        return copy_forward(dst, src, n);
    }
    return copy_backward(dst, src, n);
}

void *memset(void *restrict dst, int c, size_t n) {
    char *d = (char *)dst;
    while (n-- != 0) {
        *d++ = c;
    }
    return dst;
}
