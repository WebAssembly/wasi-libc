# LLVM emits calls to the following functions when the libcall-thread-context feature is enabled (default on WASIp3)

#include <wasi/version.h>

    .globl      __wasm_get_stack_pointer
    .type       __wasm_get_stack_pointer,@function
    .globl      __wasm_set_stack_pointer
    .type       __wasm_set_stack_pointer,@function

    .functype __wasm_component_model_builtin_context_get_0 () -> (i32)
    .import_module __wasm_component_model_builtin_context_get_0, "$root"
    .import_name __wasm_component_model_builtin_context_get_0, "[context-get-0]"
    .functype __wasm_component_model_builtin_context_set_0 (i32) -> ()
    .import_module __wasm_component_model_builtin_context_set_0, "$root"
    .import_name __wasm_component_model_builtin_context_set_0, "[context-set-0]"

__wasm_get_stack_pointer:
    .functype   __wasm_get_stack_pointer () -> (i32)
    call __wasm_component_model_builtin_context_get_0
    end_function

__wasm_set_stack_pointer:
    .functype   __wasm_set_stack_pointer (i32) -> ()
    local.get 0
    call __wasm_component_model_builtin_context_set_0
    end_function

    .globl      __wasm_get_tls_base
    .type       __wasm_get_tls_base,@function
    .globl      __wasm_set_tls_base
    .type       __wasm_set_tls_base,@function

#ifdef __wasi_cooperative_threads__
    # When coop threads are enabled the tls base lives in this async tasks's
    # context slot index as 1. Note that this slot isn't valid when the
    # component-model-threading feature is disabled, however, so it can't be
    # used.
    .functype __wasm_component_model_builtin_context_get_1 () -> (i32)
    .import_module __wasm_component_model_builtin_context_get_1, "$root"
    .import_name __wasm_component_model_builtin_context_get_1, "[context-get-1]"
    .functype __wasm_component_model_builtin_context_set_1 (i32) -> ()
    .import_module __wasm_component_model_builtin_context_set_1, "$root"
    .import_name __wasm_component_model_builtin_context_set_1, "[context-set-1]"

__wasm_get_tls_base:
    .functype   __wasm_get_tls_base () -> (i32)
    call __wasm_component_model_builtin_context_get_1
    end_function

__wasm_set_tls_base:
    .functype   __wasm_set_tls_base (i32) -> ()
    local.get 0
    call __wasm_component_model_builtin_context_set_1
    end_function
#else
    # When coop threads are disabled do something similar to the basic output
    # which is to have a global that stores the TLS base.
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
