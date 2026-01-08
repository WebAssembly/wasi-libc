	.text

	.export_name	_initialize, _initialize

	.globaltype	__init_stack_pointer, i32
    .globaltype __init_tls_base, i32

	.functype	_libc_initialize () -> ()
    
    .functype   __wasm_component_model_builtin_context_set_0 (i32) -> ()
    .import_module __wasm_component_model_builtin_context_set_0, "$root"
    .import_name __wasm_component_model_builtin_context_set_0, "[context-set-0]"
    .functype   __wasm_component_model_builtin_context_set_1 (i32) -> ()
    .import_module __wasm_component_model_builtin_context_set_1, "$root"
    .import_name __wasm_component_model_builtin_context_set_1, "[context-set-1]"

	.globl	_initialize
	.type	_initialize,@function

_initialize:
	.functype	_initialize () -> ()

    global.get   __init_stack_pointer
	call __wasm_component_model_builtin_context_set_0

    global.get   __init_tls_base
    call __wasm_component_model_builtin_context_set_1

    call	_libc_initialize

    end_function

