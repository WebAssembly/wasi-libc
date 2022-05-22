#include <unistd.h>
#include <errno.h>
#include <string.h>

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

