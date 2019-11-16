#include <wasi/api.h>
#include <wasi/libc.h>
#include <stdlib.h>
#include <sysexits.h>

// The user's `main` function, expecting arguments.
int main(int argc, char *argv[]);

// If the user's `main` function expects arguments, the compiler won't emit
// an `__original_main` function so this version will get linked in, which
// initializes the argument data and calls `main`.
int __original_main(void) {
    __wasi_errno_t err;

    // Get the sizes of the arrays we'll have to create to copy in the args.
    size_t argv_buf_size;
    size_t argc;
    err = __wasi_args_sizes_get(&argc, &argv_buf_size);
    if (err != __WASI_ERRNO_SUCCESS) {
        _Exit(EX_OSERR);
    }

    // Add 1 for the NULL pointer to mark the end, and check for overflow.
    size_t num_ptrs = argc + 1;
    if (num_ptrs == 0) {
        _Exit(EX_SOFTWARE);
    }

    // Allocate memory for storing the argument chars.
    char *argv_buf = malloc(argv_buf_size);
    if (argv_buf == NULL) {
        _Exit(EX_SOFTWARE);
    }

    // Allocate memory for the array of pointers. This uses `calloc` both to
    // handle overflow and to initialize the NULL pointer at the end.
    char **argv = calloc(num_ptrs, sizeof(char *));
    if (argv == NULL) {
        free(argv_buf);
        _Exit(EX_SOFTWARE);
    }

    // Fill the argument chars, and the argv array with pointers into those chars.
    // TODO: Remove the casts on `argv_ptrs` and `argv_buf` once the witx is updated with char8 support.
    err = __wasi_args_get((uint8_t **)argv, (uint8_t *)argv_buf);
    if (err != __WASI_ERRNO_SUCCESS) {
        free(argv_buf);
        free(argv);
        _Exit(EX_OSERR);
    }

    // Call main with the arguments!
    return main(argc, argv);
}
