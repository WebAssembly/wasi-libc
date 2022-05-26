#include <unistd.h>
#include <errno.h>
#include <string.h>

#if LONG_MAX > INT_MAX

char *getcwd(char *buf, size_t size)
{
    // Perform system call.
    uint64_t size_64 = size;
    __wasi_errno_t error = __wasi_getcwd((uint8_t *)buf, &size_64);
    if (error != 0) {
      errno = error;
      return NULL;
    }
    return buf;
}

#else

// For threads this needs to synchronize with chdir
#ifdef _REENTRANT
#error "getcwd doesn't yet support multiple threads"
#endif

char *__wasilibc_cwd = "/";

char *getcwd(char *buf, size_t size)
{
    if (!buf) {
        buf = strdup(__wasilibc_cwd);
        if (!buf) {
            errno = ENOMEM;
            return NULL;
        }
    } else {
        size_t len = strlen(__wasilibc_cwd);
        if (size < len + 1) {
            errno = ERANGE;
            return NULL;
        }
        strcpy(buf, __wasilibc_cwd);
    }
    return buf;
}

#endif