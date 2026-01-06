#ifdef _REENTRANT
#include <stdatomic.h>
#endif

#include <wasi/api.h>
#include <stdbool.h>

extern void __wasi_init_tp(void);
extern void __wasm_call_ctors(void);
extern int __main_void(void);
extern void __wasm_call_dtors(void);

__attribute__((export_name("_start")))
void _start(void) {
    // Commands should only be called once per instance. This simple check
    // ensures that the `_start` function isn't started more than once.
    //
    // We use `volatile` here to prevent the store to `started` from being
    // sunk past any subsequent code, and to prevent any compiler from
    // optimizing based on the knowledge that `_start` is the program
    // entrypoint.
#ifdef _REENTRANT
    static volatile _Atomic int started = 0;
    int expected = 0;
    if (!atomic_compare_exchange_strong(&started, &expected, 1)) {
        __builtin_trap();
    }
#else
    static volatile int started = 0;
    if (started != 0) {
        __builtin_trap();
    }
    started = 1;
#endif

    __wasi_init_tp();

    // The linker synthesizes this to call constructors.
    __wasm_call_ctors();

    // Call `__main_void` which will either be the application's zero-argument
    // `__main_void` function or a libc routine which obtains the command-line
    // arguments and calls `__main_argv_argc`.
    int r = __main_void();

    // Call atexit functions, destructors, stdio cleanup, etc.
    __wasm_call_dtors();

    // If main exited successfully, just return, otherwise call
    // `__wasi_proc_exit`.
#ifdef __wasip2__
    if (r != 0) {
        exit_result_void_void_t status = { .is_err = true };
        exit_exit(&status);
    }
#else
    if (r != 0) {
        __wasi_proc_exit(r);
    }
#endif
}


#ifdef __wasip2__
#include <wasi/libc-environ.h>

// The wasi:cli/run export for wasip2 components. This is only linked for
// command-style executables, not reactors.
bool exports_wasi_cli_run_run(void) {
    __wasilibc_initialize_environ();
    return __main_void() == 0;
}
#endif
