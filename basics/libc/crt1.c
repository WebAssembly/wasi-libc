#include <stdlib.h>
#include <sysexits.h>
#include <wasi/core.h>

extern char **environ;
extern void __wasm_call_ctors(void);
extern int main(int, char *[]);
extern void __prepare_for_exit(void);
void _Exit(int) __attribute__((noreturn));

static __wasi_errno_t populate_args(size_t *argc, char **argv) {
    __wasi_errno_t err;

    /* Get the sizes of the arrays we'll have to create to copy in the args. */
    size_t argv_size;
    err = __wasi_arg_sizes_get(argc, &argv_size);
    if (err != __WASI_ESUCCESS) {
        return err;
    }

    /* Allocate memory for the array of pointers. */
    argv = malloc(sizeof(char *) * *argc);
    /* Allocate memory for storing the argument chars. */
    char *argv_buf = malloc(sizeof(char) * argv_size);
    if (argv == NULL || argv_buf == NULL) {
        return __WASI_ENOMEM;
    }

    /* Fill the argument chars, and the argv array with pointers into those chars. */
    return __wasi_argv_get(argv, argv_buf);
}

static __wasi_errno_t populate_environ() {
    __wasi_errno_t err;

    /* Get the sizes of the arrays we'll have to create to copy in the environment. */
    size_t environ_count;
    size_t environ_size;
    err = __wasi_environ_sizes_get(&environ_count, &environ_size);
    if (err != __WASI_ESUCCESS) {
        return err;
    }

    /* Allocate memory for the array of pointers. */
    environ = malloc(sizeof(char *) * environ_count);
    /* Allocate memory for storing the environment chars. */
    char *environ_buf = malloc(sizeof(char) * environ_size);
    if (environ == NULL || environ_buf == NULL) {
        return __WASI_ENOMEM;
    }

    /* Fill the environment chars, and the __environ array with pointers into those chars. */
    return __wasi_environ_get(environ, environ_buf);
}

void _start(void) {
    /* The linker synthesizes this to call constructors. */
    __wasm_call_ctors();

    /* Fill in the environment from WASI syscalls. */
    if (populate_environ() != __WASI_ESUCCESS) {
        _Exit(EX_OSERR);
    }

    /* Fill in the arguments from WASI syscalls. */
    size_t argc;
    char **argv;
    if (populate_args(&argc, argv) != __WASI_ESUCCESS) {
        _Exit(EX_OSERR);
    }

    /* Call main with the arguments. */
    int r = main(argc, argv);

    /* Call atexit functions, destructors, stdio cleanup, etc. */
    __prepare_for_exit();

    /* If main exited successfully, just return, otherwise call _Exit. */
    if (r != 0) {
        _Exit(r);
    }
}
