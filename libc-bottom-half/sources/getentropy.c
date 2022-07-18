#include <wasi/api.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>

int __getentropy(void *buffer, size_t len) {
    int cs;
    if (len > 256) {
        errno = EIO;
        return -1;
    }

    pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &cs);
    int r = __wasi_random_get(buffer, len);
    pthread_setcancelstate(cs, 0);

    if (r != 0) {
        errno = r;
        return -1;
    }

    return 0;
}
weak_alias(__getentropy, getentropy);
