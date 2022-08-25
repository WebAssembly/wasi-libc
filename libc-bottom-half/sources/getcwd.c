#include <unistd.h>
#include <errno.h>
#include <string.h>

char *getcwd(char *buf, size_t size)
{
    __wasi_pointersize_t bufsize = size;
    __wasi_errno_t error = __wasi_getcwd((uint8_t *)buf, &bufsize);
    size = (size_t)bufsize;
    if (error != 0) {
      errno = error;
      return NULL;
    }
    return buf;
}