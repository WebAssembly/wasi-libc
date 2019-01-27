#include <wasi.h>
#include <errno.h>

#ifdef _REENTRANT
#error With threads support, getentropy is not intended to be a cancellation point.
#endif

int getentropy(void *buffer, size_t len) {
    if (len > 256) {
        errno = EIO;
        return -1;
    }

    int r = wasi_random_get(buffer, len);

    if (r != 0) {
        errno = r;
        return -1;
    }

    return 0;
}
