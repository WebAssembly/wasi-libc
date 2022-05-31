#include <wasi/api.h>
#include <stdlib.h>
extern void __wasm_call_ctors(void);
extern int __main_void(void);
extern void __wasm_call_dtors(void);

__attribute__((export_name("_start")))
void _start(void) {
    // Call `__main_void` which will either be the application's zero-argument
    // `__main_void` function or a libc routine which obtains the command-line
    // arguments and calls `__main_argv_argc`.
    int r = __main_void();

    // If main exited successfully, just return, otherwise call `exit`.
    if (r != 0) {
        exit(r);
    }
}
