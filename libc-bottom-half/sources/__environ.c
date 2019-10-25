#include <unistd.h>
#include <stdlib.h>
#include <wasi/core.h>
#include <wasi/libc.h>
#include <wasi/libc-internal.h>

static char *empty_environ[1] = { NULL };
char **__environ = empty_environ;
extern __typeof(__environ) _environ __attribute__((weak, alias("__environ")));
extern __typeof(__environ) environ __attribute__((weak, alias("__environ")));

// This function is referenced by a weak symbol in crt1.c, and we define
// it here in the same source file as __environ, so that this function is
// linked in iff environment variable support is used.
__wasi_errno_t __wasilibc_populate_environ(void) {
    __wasi_errno_t err;

    // Get the sizes of the arrays we'll have to create to copy in the environment.
    size_t environ_count;
    size_t environ_buf_size;
    err = __wasi_environ_sizes_get(&environ_count, &environ_buf_size);
    if (err != __WASI_ESUCCESS) {
        return err;
    }
    if (environ_count == 0) {
        return __WASI_ESUCCESS;
    }

    // Add 1 for the NULL pointer to mark the end, and check for overflow.
    size_t num_ptrs = environ_count + 1;
    if (num_ptrs == 0) {
        return __WASI_ENOMEM;
    }

    // Allocate memory for storing the environment chars.
    char *environ_buf = malloc(environ_buf_size);
    if (environ_buf == NULL) {
        return __WASI_ENOMEM;
    }

    // Allocate memory for the array of pointers. This uses `calloc` both to
    // handle overflow and to initialize the NULL pointer at the end.
    char **environ_ptrs = calloc(num_ptrs, sizeof(char *));
    if (environ_ptrs == NULL) {
        free(environ_buf);
        return __WASI_ENOMEM;
    }

    // Fill the environment chars, and the __environ array with pointers into those chars.
    err = __wasi_environ_get(environ_ptrs, environ_buf);
    if (err == __WASI_ESUCCESS) {
        __environ = environ_ptrs;
    } else {
        free(environ_buf);
        free(environ_ptrs);
    }
    return err;
}
