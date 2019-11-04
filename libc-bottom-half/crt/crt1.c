#include <stdlib.h>
#include <sysexits.h>
#include <wasi/core.h>
#include <wasi/libc.h>
#include <wasi/libc-internal.h>

__wasi_errno_t __wasilibc_populate_environ(void) __attribute__((weak));
extern void __wasm_call_ctors(void);
extern int __original_main(void);
extern void __prepare_for_exit(void);
void _Exit(int) __attribute__((noreturn));

static __wasi_errno_t populate_libpreopen(void) {
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

    // The linker synthesizes this to call constructors.
    __wasm_call_ctors();

    // Call `__original_main` which will either be the application's
    // zero-argument `main` function (renamed by the compiler) or a libc
    // routine which populates `argv` and `argc` and calls the application's
    // two-argument `main`.
    int r = __original_main();

    // Call atexit functions, destructors, stdio cleanup, etc.
    __prepare_for_exit();

    // If main exited successfully, just return, otherwise call _Exit.
    if (r != 0) {
        _Exit(r);
    }
}
