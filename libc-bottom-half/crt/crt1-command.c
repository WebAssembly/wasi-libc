#ifdef _REENTRANT
#include <stdatomic.h>
extern void __wasi_init_tp(void);
#endif
#include <wasi/api.h>
extern void __wasm_call_ctors(void);
extern int __main_void(void) __attribute__((weak));
extern int __main_argc_argv(int argc, char *argv[]) __attribute__((weak));
extern int __call_main_argc_argv(int (*main)(int argc, char *argv[]));
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

#ifdef _REENTRANT
	__wasi_init_tp();
#endif

    // The linker synthesizes this to call constructors.
    __wasm_call_ctors();

    // Call `__main_void` or `__call_main_argc_argv`, depending on which kind
    // of `main` the program has.
    int r;
    if (__main_void) {
        r = __main_void();
    } else if (__main_argc_argv) {
        r = __call_main_argc_argv(__main_argc_argv);
    } else {
        // Either `__main_void` or `__main_argc_argv` should be defined.
        __builtin_trap();
    }

    // Call atexit functions, destructors, stdio cleanup, etc.
    __wasm_call_dtors();

    // If main exited successfully, just return, otherwise call
    // `__wasi_proc_exit`.
    if (r != 0) {
        __wasi_proc_exit(r);
    }
}
