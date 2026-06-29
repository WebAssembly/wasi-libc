# LLVM emits calls to the following functions when the libcall-thread-context feature is enabled (default on WASIp3)

    .globl      __wasm_get_stack_pointer
    .type       __wasm_get_stack_pointer,@function
    .globl      __wasm_set_stack_pointer
    .type       __wasm_set_stack_pointer,@function
    .globl      __wasm_get_tls_base
    .type       __wasm_get_tls_base,@function
    .globl      __wasm_set_tls_base
    .type       __wasm_set_tls_base,@function

    .functype __wasm_component_model_builtin_context_get_0 () -> (i32)
    .import_module __wasm_component_model_builtin_context_get_0, "$root"
    .import_name __wasm_component_model_builtin_context_get_0, "[context-get-0]"
    .functype __wasm_component_model_builtin_context_set_0 (i32) -> ()
    .import_module __wasm_component_model_builtin_context_set_0, "$root"
    .import_name __wasm_component_model_builtin_context_set_0, "[context-set-0]"
    .functype __wasm_component_model_builtin_context_get_1 () -> (i32)
    .import_module __wasm_component_model_builtin_context_get_1, "$root"
    .import_name __wasm_component_model_builtin_context_get_1, "[context-get-1]"
    .functype __wasm_component_model_builtin_context_set_1 (i32) -> ()
    .import_module __wasm_component_model_builtin_context_set_1, "$root"
    .import_name __wasm_component_model_builtin_context_set_1, "[context-set-1]"

__wasm_get_stack_pointer:
    .functype   __wasm_get_stack_pointer () -> (i32)
    call __wasm_component_model_builtin_context_get_0
    end_function

__wasm_set_stack_pointer:
    .functype   __wasm_set_stack_pointer (i32) -> ()
    local.get 0
    call __wasm_component_model_builtin_context_set_0
    end_function

__wasm_get_tls_base:
    .functype   __wasm_get_tls_base () -> (i32)
    call __wasm_component_model_builtin_context_get_1
    end_function

__wasm_set_tls_base:
    .functype   __wasm_set_tls_base (i32) -> ()
    local.get 0
    call __wasm_component_model_builtin_context_set_1
    end_function
