#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include "lock.h"

char *__wasilibc_cwd = "/";

#ifdef _REENTRANT
static volatile int lock[1];
void __wasilibc_cwd_lock(void) { LOCK(lock); }
void __wasilibc_cwd_unlock(void) { UNLOCK(lock); }
#else
#define __wasilibc_cwd_lock() (void)0
#define __wasilibc_cwd_unlock() (void)0
#endif

char *getcwd(char *buf, size_t size)
{
    __wasi_pointersize_t bufsize = size;
    __wasi_errno_t error = __wasi_getcwd((uint8_t *)buf, &bufsize);
    if (error == ERANGE && buf == NULL) {
        buf = (char *)__libc_malloc(bufsize);
        if (buf == NULL) {
            return NULL;
        }
        error = __wasi_getcwd((uint8_t *)buf, &bufsize);
    }
    
    if (error != 0) {
      errno = error;
      return NULL;
    }
    return buf;
}

char *getcwd_legacy(char *buf, size_t size)
{
    __wasilibc_cwd_lock();
    if (!buf) {
        buf = strdup(__wasilibc_cwd);
        if (!buf) {
            errno = ENOMEM;
            __wasilibc_cwd_unlock();
            return NULL;
        }
    } else {
        size_t len = strlen(__wasilibc_cwd);
        if (size < len + 1) {
            errno = ERANGE;
            __wasilibc_cwd_unlock();
            return NULL;
        }
        strcpy(buf, __wasilibc_cwd);
    }
    __wasilibc_cwd_unlock();
    return buf;
}