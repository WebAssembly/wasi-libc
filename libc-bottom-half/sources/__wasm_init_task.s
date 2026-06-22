    .text

	.export_name	__wasm_init_task, __wasm_init_task
    .export_name    __wasm_init_async_task, __wasm_init_async_task

	.globaltype	__init_stack_pointer, i32, immutable
    .globaltype __init_tls_base, i32, immutable
    .globaltype __tls_size, i32, immutable
    .globaltype __tls_align, i32, immutable

    .functype   __wasm_set_stack_pointer (i32) -> ()
    .functype   __wasm_set_tls_base (i32) -> ()

    .functype    malloc (i32) -> (i32)
    .functype   __copy_tls (i32) -> (i32)
    .functype   __allocate_tls () -> (i32)

	.globl	__wasm_init_task
	.type	__wasm_init_task,@function
    .globl	__wasm_init_async_task
    .type	__wasm_init_async_task,@function

	.functype	__wasi_init_tp() -> () 

__wasm_init_task:
	.functype	__wasm_init_task () -> ()

    global.get   __init_stack_pointer
	call __wasm_set_stack_pointer

    global.get   __init_tls_base
    call __wasm_set_tls_base

    # Allocate a new TLS area
    call __allocate_tls
    call __copy_tls
    call __wasm_set_tls_base

    call __wasi_init_tp

    end_function

__wasm_init_async_task:
    .functype	__wasm_init_async_task () -> ()

    # malloc and __init_tls may use the stack pointer and TLS base, so set those first
    # to the statically-allocated values used for synchronous tasks.
    global.get   __init_stack_pointer
    call __wasm_set_stack_pointer

    global.get   __init_tls_base
    call __wasm_set_tls_base

    # Allocate a new stack
    # Constant copied from __default_stacksize, TODO(wasip3) find a way
    # to reference this constant without linking failing for shared libraries,
    # i.e. a position-independent data reference. 
    i32.const 131072 
    call malloc
    call __wasm_set_stack_pointer

    # Allocate a new TLS area
    call __allocate_tls
    call __copy_tls
    call __wasm_set_tls_base

    call __wasi_init_tp

    end_function