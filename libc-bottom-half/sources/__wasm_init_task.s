	.text

	.export_name	__wasm_init_task, __wasm_init_task
    .export_name    __wasm_init_async_task, __wasm_init_async_task

	.globaltype	__init_stack_pointer, i32
    .globaltype __init_tls_base, i32
    .globaltype __tls_size, i32, immutable
    .globaltype __tls_align, i32, immutable
    
    .functype   __wasm_component_model_builtin_context_set_0 (i32) -> ()
    .import_module __wasm_component_model_builtin_context_set_0, "$root"
    .import_name __wasm_component_model_builtin_context_set_0, "[context-set-0]"
    .functype   __wasm_component_model_builtin_context_set_1 (i32) -> ()
    .import_module __wasm_component_model_builtin_context_set_1, "$root"
    .import_name __wasm_component_model_builtin_context_set_1, "[context-set-1]"

    .functype    malloc (i32) -> (i32)
    .functype   __copy_tls (i32) -> (i32)

	.globl	__wasm_init_task
	.type	__wasm_init_task,@function
    .globl	__wasm_init_async_task
    .type	__wasm_init_async_task,@function
    
__wasm_init_task:
	.functype	__wasm_init_task () -> ()

    global.get   __init_stack_pointer
	call __wasm_component_model_builtin_context_set_0

    global.get   __init_tls_base
    call __wasm_component_model_builtin_context_set_1

    # Allocate a new TLS area
    global.get   __tls_size
    # TODO align
    call malloc
    call __copy_tls
    call __wasm_component_model_builtin_context_set_1

    end_function

__wasm_init_async_task:
    .functype	__wasm_init_async_task () -> ()

    # malloc and __init_tls may use the stack pointer and TLS base, so set those first
    # to the statically-allocated values used for synchronous tasks.
    global.get   __init_stack_pointer
    call __wasm_component_model_builtin_context_set_0

    global.get   __init_tls_base
    call __wasm_component_model_builtin_context_set_1

    # Allocate a new stack
    # Value chosen for parity with __default_stacksize in pthread_impl.h
    i32.const 131072
    call malloc
    call __wasm_component_model_builtin_context_set_0

    # Allocate a new TLS area
    global.get   __tls_size
    # TODO align
    call malloc
    call __copy_tls
    call __wasm_component_model_builtin_context_set_1

    end_function
