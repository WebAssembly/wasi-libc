#include <unistd.h>
#include <stdlib.h>
#include <sysexits.h>
#include <wasi/api.h>
#include <wasi/libc.h>
#include <wasi/libc-environ.h>

// If the program does use `environ`, it'll get this version of
// `__wasilibc_environ`, which is initialized with a constructor function, so
// that it's initialized whenever user code might want to access it.
char **__wasilibc_environ;
extern __typeof(__wasilibc_environ) _environ
    __attribute__((__weak__, alias("__wasilibc_environ")));
extern __typeof(__wasilibc_environ) environ
    __attribute__((__weak__, alias("__wasilibc_environ")));

// WebAssembly memory starts zeroed - thus we need this check
volatile int __wasilibc_environ_init __attribute__((__weak__)) = (int)0;

// We define this function here in the same source file as
// `__wasilibc_environ`, so that this function is called in iff environment
// variable support is used.
// Concerning the 50 -- levels up to 100 are reserved for the implementation,
// so we an arbitrary number in the middle of the range to allow other
// reserved things to go before or after.
__attribute__((constructor(50)))
static void __wasilibc_initialize_environ_eagerly(void) {
    if (__wasilibc_environ_init == 0) {
        __wasilibc_environ_init = 1;
        __wasilibc_initialize_environ();
    } else {
        __wasilibc_ensure_environ();
    }
}

// See the comments in libc-environ.h.
void __wasilibc_maybe_reinitialize_environ_eagerly(void) {
    // This translation unit is linked in if `environ` is used, meaning we need
    // to eagerly reinitialize the environment variables.
    __wasilibc_initialize_environ();
}
