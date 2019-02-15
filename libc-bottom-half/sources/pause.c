#include <unistd.h>
#include <wasi.h>

int pause(void) {
    size_t n;
    (void)__wasi_poll_oneoff(0, 0, 0, &n);
}
