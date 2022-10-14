#include <wasi/api.h>
extern void __wasm_call_ctors(void);
extern int __main_void(void);
extern void __wasm_call_dtors(void);

__attribute__((export_name("_start")))
void _start(void) {
    // Commands should only be called once per instance. This simple check
    // ensures that the `_start` function isn't started more than once.
    static volatile int started = 0;
#ifdef _REENTRANT
    if (__sync_val_compare_and_swap(&started, 0, 1) != 0) {
	__builtin_trap();
    }
#else
    if (started != 0) {
	__builtin_trap();
    }
    started = 1;
#endif

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
    if (r != 0) {
        __wasi_proc_exit(r);
    }
}
