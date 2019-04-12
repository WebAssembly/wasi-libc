#include <string.h>
#include <stdint.h>
#include <stdlib.h>

size_t strlen(const char *str) {
    const char *s = str;
    while (*s) {
        ++s;
    }
    return s - str;
}

char *strdup(const char *str) {
    size_t buf_len = strlen(str) + 1;
    void *ptr = malloc(buf_len);
    if (ptr == NULL) {
        return NULL;
    }
    return memcpy(ptr, str, buf_len);
}

int strcmp(const char *a, const char *b) {
    while (*a != '\0' && (*a == *b)) {
        ++a;
        ++b;
    }
    return *(const unsigned char*)a - *(const unsigned char*)b;
}

void *memchr(const void *ptr, int c, size_t len) {
    const unsigned char *p = ptr;
    while (len != 0 && *p != (unsigned char)c) {
        ++p;
        --len;
    }
    if (len == 0) {
        return NULL;
    }
    return (void *)p;
}
