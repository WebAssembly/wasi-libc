#include <unistd.h>
#include <stdlib.h>
#include <sysexits.h>
#include <wasi/api.h>
#include <wasi/libc.h>

static char *empty_environ[1] = { NULL };
char **__environ = empty_environ;
extern __typeof(__environ) _environ __attribute__((weak, alias("__environ")));
extern __typeof(__environ) environ __attribute__((weak, alias("__environ")));

// We define this function here in the same source file as __environ, so that
// this function is called in iff environment variable support is used.
// Concerning the 50 -- levels up to 100 are reserved for the implementation,
// so we an arbitrary number in the middle of the range to allow other
// reserved things to go before or after.
__attribute__((constructor(50)))
static void __wasilibc_populate_environ(void) {
    __wasi_errno_t err;

    // Get the sizes of the arrays we'll have to create to copy in the environment.
    size_t environ_count;
    size_t environ_buf_size;
    err = __wasi_environ_sizes_get(&environ_count, &environ_buf_size);
    if (err != __WASI_ERRNO_SUCCESS) {
        goto oserr;
    }
    if (environ_count == 0) {
        return;
    }

    // Add 1 for the NULL pointer to mark the end, and check for overflow.
    size_t num_ptrs = environ_count + 1;
    if (num_ptrs == 0) {
        goto software;
    }

    // Allocate memory for storing the environment chars.
    char *environ_buf = malloc(environ_buf_size);
    if (environ_buf == NULL) {
        goto software;
    }

    // Allocate memory for the array of pointers. This uses `calloc` both to
    // handle overflow and to initialize the NULL pointer at the end.
    char **environ_ptrs = calloc(num_ptrs, sizeof(char *));
    if (environ_ptrs == NULL) {
        free(environ_buf);
        goto software;
    }

    // Fill the environment chars, and the __environ array with pointers into those chars.
    // TODO: Remove the casts on `environ_ptrs` and `environ_buf` once the witx is updated with char8 support.
    err = __wasi_environ_get((uint8_t **)environ_ptrs, (uint8_t *)environ_buf);
    if (err != __WASI_ERRNO_SUCCESS) {
        free(environ_buf);
        free(environ_ptrs);
        goto oserr;
    }

    __environ = environ_ptrs;
    return;
oserr:
    _Exit(EX_OSERR);
software:
    _Exit(EX_SOFTWARE);
}
