extern void __wasm_call_ctors(void);

void _activate(void) {
    // The linker synthesizes this to call constructors.
    __wasm_call_ctors();
}
