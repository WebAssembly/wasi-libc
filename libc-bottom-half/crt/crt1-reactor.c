#if defined(_REENTRANT)
#include <stdatomic.h>
#endif
extern void __wasi_init_tp(void);
extern void __wasm_call_ctors(void);

// On WASIP3, the _initialize function is implemented in crt/wasm32/wasip3-crt1-reactor-start.s
// so that it can initialize the stack pointer and TLS before calling the real
// _initialize function (renamed to _libc_initialize here).
#ifdef __wasip3__
void _libc_initialize(void)
#else
__attribute__((export_name("_initialize"))) void _initialize(void)
#endif
{
#if defined(_REENTRANT)
    static volatile atomic_int initialized = 0;
    int expected = 0;
    if (!atomic_compare_exchange_strong(&initialized, &expected, 1))
    {
        __builtin_trap();
    }
#else
    static volatile int initialized = 0;
    if (initialized != 0)
    {
        __builtin_trap();
    }
    initialized = 1;
#endif
    __wasi_init_tp();

    // The linker synthesizes this to call constructors.
    __wasm_call_ctors();
}
