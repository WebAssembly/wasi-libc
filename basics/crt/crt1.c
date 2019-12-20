extern void __wasm_call_ctors(void);
extern int __original_main(void);
extern void __prepare_for_exit(void);
void _Exit(int) __attribute__((noreturn));

void _start(void) {
    // The linker synthesizes this to call constructors.
    __wasm_call_ctors();

    // Call `__original_main` which will either be the application's zero-argument
    // `__original_main` function or a libc routine which calls `__main_void`.
    // TODO: Call `main` directly once we no longer have to support old compilers.
    int r = __original_main();

    // Call atexit functions, destructors, stdio cleanup, etc.
    __prepare_for_exit();

    // If main exited successfully, just return, otherwise call _Exit.
    if (r != 0) {
        _Exit(r);
    }
}
