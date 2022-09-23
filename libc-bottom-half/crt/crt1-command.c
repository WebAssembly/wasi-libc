#include <wasi/api.h>
extern void __wasm_call_ctors(void);
extern int __original_main(void);
extern void __wasm_call_dtors(void);

__attribute__((export_name("_start")))
void _start(void) {
    // The linker synthesizes this to call constructors.
    __wasm_call_ctors();

    // Call `__main_void` which will either be the application's zero-argument
    // `__main_void` function or a libc routine which obtains the command-line
    // arguments and calls `__main_argv_argc`.
    int r = __original_main();

    // Call atexit functions, destructors, stdio cleanup, etc.
    __wasm_call_dtors();

    // If main exited successfully, just return, otherwise call
    // `__wasi_proc_exit`.
    if (r != 0) {
        __wasi_proc_exit(r);
    }
}
