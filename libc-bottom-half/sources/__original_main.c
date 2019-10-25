#include <wasi/core.h>
#include <wasi/libc.h>
#include <alloca.h>
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
    if (err != __WASI_ESUCCESS) {
        _Exit(EX_OSERR);
    }

    // Add 1 for the NULL pointer to mark the end, and check for overflow.
    size_t num_ptrs = argc + 1;
    if (num_ptrs == 0) {
        _Exit(EX_SOFTWARE);
    }

    // Allocate memory for storing the argument chars.
    char *argv_buf = alloca(argv_buf_size);

    // Allocate memory for the array of pointers.
    size_t num_ptrs_size;
    if (__builtin_mul_overflow(num_ptrs, sizeof(char *), &num_ptrs_size)) {
        _Exit(EX_SOFTWARE);
    }
    char **argv = alloca(num_ptrs_size);

    // Fill the argument chars, and the argv array with pointers into those chars.
    err = __wasi_args_get(argv, argv_buf);
    if (err != __WASI_ESUCCESS) {
        _Exit(EX_OSERR);
    }

    // Make sure the last pointer in the array is NULL.
    argv[argc] = NULL;

    // Call main with the arguments!
    return main(argc, argv);
}
