#if defined(_REENTRANT)
#include <stdatomic.h>
extern void __wasi_init_tp(void);
#endif
extern void __wasm_call_ctors(void);

__attribute__((export_name("_initialize")))
void _initialize(void) {
#ifdef WASI_PREVIEW2
    // Link in the bindings.
    extern void __component_type_object_force_link_command_public_use_in_this_compilation_unit(void);
    __component_type_object_force_link_command_public_use_in_this_compilation_unit();
#endif

#if defined(_REENTRANT)
    static volatile atomic_int initialized = 0;
    int expected = 0;
    if (!atomic_compare_exchange_strong(&initialized, &expected, 1)) {
        __builtin_trap();
    }

    __wasi_init_tp();
#else
    static volatile int initialized = 0;
    if (initialized != 0) {
        __builtin_trap();
    }
    initialized = 1;
#endif

    // The linker synthesizes this to call constructors.
    __wasm_call_ctors();
}
