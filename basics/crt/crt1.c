extern void __wasm_call_ctors(void);
extern int main(int, char *[]);
extern void __prepare_for_exit(void);
void _Exit(int) __attribute__((noreturn));

void _start(void) {
    /* The linker synthesizes this to call constructors. */
    __wasm_call_ctors();

    /* Call main with no arguments. */
    int r = main(0, 0);

    /* Call atexit functions, destructors, stdio cleanup, etc. */
    __prepare_for_exit();

    /* If main exited successfully, just return, otherwise call _Exit. */
    if (r != 0) {
        _Exit(r);
    }
}
