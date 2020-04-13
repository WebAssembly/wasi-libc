extern void __wasm_call_ctors(void);

__attribute__((export_name("_activate")))
void _activate(void) {
    // The linker synthesizes this to call constructors.
    __wasm_call_ctors();
}
