// On WASIPp3 when coop threads are disabled the `__wasm_{g,s}et_tls_base`
// symbols are defined here instead of imported into the module to avoid using
// context slot 1. The definition here is in terms of a local wasm global.

#include <wasi/version.h>

#if defined(__wasm_libcall_thread_context__) && !defined(__wasi_cooperative_threads__)
    .globl      __wasm_get_tls_base
    .type       __wasm_get_tls_base,@function
    .globl      __wasm_set_tls_base
    .type       __wasm_set_tls_base,@function

    // When coop threads are disabled do something similar to the basic output
    // which is to have a global that stores the TLS base.
    .globaltype __wasilibc_tls_base, i32

__wasm_get_tls_base:
    .functype   __wasm_get_tls_base () -> (i32)
    global.get __wasilibc_tls_base
    end_function

__wasm_set_tls_base:
    .functype   __wasm_set_tls_base (i32) -> ()
    local.get 0
    global.set __wasilibc_tls_base
    end_function

    .section .wasm.__wasilibc_tls_base,"",@
__wasilibc_tls_base:
#endif
