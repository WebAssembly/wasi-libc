#include <stddef.h>
#include <errno.h>
#include <wasi/core.h>
#include <unistd.h>

int pause(void) {
    size_t n;
    __wasi_errno_t error = __wasi_poll_oneoff(0, 0, 0, &n);
    if (error != 0) {
        errno = error;
        return -1;
    }
    __builtin_trap();
}
