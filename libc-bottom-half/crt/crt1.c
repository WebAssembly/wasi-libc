#include <stdlib.h>
#include <sysexits.h>
#include <wasi/core.h>
#include <wasi/libc.h>

extern char **__environ;
extern void __wasm_call_ctors(void);
extern int main(int, char *[]);
extern void __prepare_for_exit(void);
void _Exit(int) __attribute__((noreturn));

static __wasi_errno_t populate_args(size_t *argc, char ***argv) {
    __wasi_errno_t err;

    /* Get the sizes of the arrays we'll have to create to copy in the args. */
    size_t argv_buf_size;
    err = __wasi_args_sizes_get(argc, &argv_buf_size);
    if (err != __WASI_ESUCCESS) {
        return err;
    }
    if (*argc == 0) {
        return __WASI_ESUCCESS;
    }

    /* Allocate memory for the array of pointers. */
    *argv = malloc(sizeof(char *) * *argc);
    /* Allocate memory for storing the argument chars. */
    char *argv_buf = malloc(sizeof(char) * argv_buf_size);
    if (*argv == NULL || argv_buf == NULL) {
        return __WASI_ENOMEM;
    }

    /* Fill the argument chars, and the argv array with pointers into those chars. */
    return __wasi_args_get(*argv, argv_buf);
}

static __wasi_errno_t populate_environ(void) {
    __wasi_errno_t err;

    /* Get the sizes of the arrays we'll have to create to copy in the environment. */
    size_t environ_count;
    size_t environ_buf_size;
    err = __wasi_environ_sizes_get(&environ_count, &environ_buf_size);
    if (err != __WASI_ESUCCESS) {
        return err;
    }
    /* If there's no environment pairs, make sure environ is null and return. */
    if (environ_count == 0) {
        __environ = NULL;
        return __WASI_ESUCCESS;
    }

    /* Allocate memory for the array of pointers, plus one terminating null pointer. */
    __environ = malloc(sizeof(char *) * (environ_count + 1));
    /* Allocate memory for storing the environment chars. */
    char *environ_buf = malloc(sizeof(char) * environ_buf_size);
    if (__environ == NULL || environ_buf == NULL) {
        return __WASI_ENOMEM;
    }

    /* Make sure the last pointer in the array is NULL. */
    __environ[environ_count] = NULL;

    /* Fill the environment chars, and the __environ array with pointers into those chars. */
    return __wasi_environ_get(__environ, environ_buf);
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
    /* Record the preopened resources. */
    if (populate_libpreopen() != __WASI_ESUCCESS) {
        _Exit(EX_OSERR);
    }

    /* Fill in the environment from WASI syscalls. */
    if (populate_environ() != __WASI_ESUCCESS) {
        _Exit(EX_OSERR);
    }

    /* Fill in the arguments from WASI syscalls. */
    size_t argc;
    char **argv;
    if (populate_args(&argc, &argv) != __WASI_ESUCCESS) {
        _Exit(EX_OSERR);
    }

    /* The linker synthesizes this to call constructors. */
    __wasm_call_ctors();

    /* Call main with the arguments. */
    int r = main(argc, argv);

    /* Call atexit functions, destructors, stdio cleanup, etc. */
    __prepare_for_exit();

    /* If main exited successfully, just return, otherwise call _Exit. */
    if (r != 0) {
        _Exit(r);
    }
}
