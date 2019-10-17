#include <stdlib.h>
#include <sysexits.h>
#include <wasi/core.h>
#include <wasi/libc.h>
#include <wasi/libc-internal.h>

__wasi_errno_t __wasilibc_populate_environ(void) __attribute__((weak));
extern void __wasm_call_ctors(void);
extern int main(int, char *[]);
extern void __prepare_for_exit(void);
void _Exit(int) __attribute__((noreturn));

static __wasi_errno_t populate_args(size_t *argc, char ***argv) {
    __wasi_errno_t err;

    // Get the sizes of the arrays we'll have to create to copy in the args.
    size_t argv_buf_size;
    size_t new_argc;
    err = __wasi_args_sizes_get(&new_argc, &argv_buf_size);
    if (err != __WASI_ESUCCESS) {
        return err;
    }
    if (new_argc == 0) {
        return __WASI_ESUCCESS;
    }

    // Add 1 for the NULL pointer to mark the end, and check for overflow.
    size_t num_ptrs = new_argc + 1;
    if (num_ptrs == 0) {
        return __WASI_ENOMEM;
    }

    // Allocate memory for storing the argument chars.
    char *argv_buf = malloc(argv_buf_size);
    if (argv_buf == NULL) {
        return __WASI_ENOMEM;
    }

    // Allocate memory for the array of pointers. This uses `calloc` both to
    // handle overflow and to initialize the NULL pointer at the end.
    char **argv_ptrs = calloc(num_ptrs, sizeof(char *));
    if (argv_ptrs == NULL) {
        free(argv_buf);
        return __WASI_ENOMEM;
    }

    // Fill the argument chars, and the argv array with pointers into those chars.
    err = __wasi_args_get(argv_ptrs, argv_buf);
    if (err == __WASI_ESUCCESS) {
        *argc = new_argc;
        *argv = argv_ptrs;
    } else {
        free(argv_buf);
        free(argv_ptrs);
    }
    return err;
}

static __wasi_errno_t populate_libpreopen(void) {
    __wasilibc_init_preopen();

    // Skip stdin, stdout, and stderr, and count up until we reach an invalid
    // file descriptor.
    for (__wasi_fd_t fd = 3; fd != 0; ++fd) {
        __wasi_prestat_t prestat;
        __wasi_errno_t ret = __wasi_fd_prestat_get(fd, &prestat);
        if (ret == __WASI_EBADF)
            break;
        if (ret != __WASI_ESUCCESS)
            return ret;
        switch (prestat.pr_type) {
        case __WASI_PREOPENTYPE_DIR: {
            char *path = malloc(prestat.u.dir.pr_name_len + 1);
            if (path == NULL)
                return __WASI_ENOMEM;

            ret = __wasi_fd_prestat_dir_name(fd, path, prestat.u.dir.pr_name_len);
            if (ret != __WASI_ESUCCESS) {
                free(path);
                return ret;
            }
            path[prestat.u.dir.pr_name_len] = '\0';

            if (__wasilibc_register_preopened_fd(fd, path) != 0) {
                free(path);
                return __WASI_ENOMEM;
            }

            free(path);
            break;
        }
        default:
            break;
        }
    }

    return __WASI_ESUCCESS;
}

void _start(void) {
    // Record the preopened resources.
    if (populate_libpreopen() != __WASI_ESUCCESS) {
        _Exit(EX_OSERR);
    }

    // Fill in the environment from WASI syscalls, if needed.
    if (&__wasilibc_populate_environ != NULL) {
        if (__wasilibc_populate_environ() != __WASI_ESUCCESS) {
            _Exit(EX_OSERR);
        }
    }

    // Fill in the arguments from WASI syscalls.
    size_t argc;
    char **argv;
    if (populate_args(&argc, &argv) != __WASI_ESUCCESS) {
        _Exit(EX_OSERR);
    }

    // The linker synthesizes this to call constructors.
    __wasm_call_ctors();

    // Call main with the arguments.
    int r = main(argc, argv);

    // Call atexit functions, destructors, stdio cleanup, etc.
    __prepare_for_exit();

    // If main exited successfully, just return, otherwise call _Exit.
    if (r != 0) {
        _Exit(r);
    }
}
