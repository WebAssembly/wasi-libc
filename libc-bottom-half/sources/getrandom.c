#include <wasi/api.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>

ssize_t getrandom(void *buf, size_t buflen, unsigned flags) {
    int cs;
    pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &cs);
    int r = __wasi_random_get(buf, buflen);
    pthread_setcancelstate(cs, 0);

    if (r != 0) {
        errno = r;
        return -1;
    }

    return 0;
}
