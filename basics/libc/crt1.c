extern int main(int, char *[]);
void _Exit(int) __attribute__((__noreturn__));
extern void __prepare_for_exit(void);
extern void __wasm_call_ctors(void);

void _start(void) {
    __wasm_call_ctors();

    /* For now, just call main with no arguments. */
    int r = main(0, 0);

    __prepare_for_exit();

    if (r != 0) {
        _Exit(r);
    }
}
