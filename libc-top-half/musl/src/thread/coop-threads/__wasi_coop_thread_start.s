	.text

	.export_name	__wasi_coop_thread_start, __wasi_coop_thread_start

	.globaltype	__init_stack_pointer, i32
	.functype	__wasi_coop_thread_start_C (i32, i32) -> ()
	.functype   __wasm_component_model_builtin_context_set_0 (i32) -> ()
    .import_module __wasm_component_model_builtin_context_set_0, "$root"
    .import_name __wasm_component_model_builtin_context_set_0, "[context-set-0]"
    .functype   __wasm_component_model_builtin_context_set_1 (i32) -> ()
    .import_module __wasm_component_model_builtin_context_set_1, "$root"
    .import_name __wasm_component_model_builtin_context_set_1, "[context-set-1]"
	.import_module __wasm_component_model_builtin_thread_index, "$root"
	.import_name __wasm_component_model_builtin_thread_index, "[thread-index]"
    .functype   __wasm_component_model_builtin_thread_index () -> (i32)

	.hidden	__wasi_coop_thread_start
	.globl	__wasi_coop_thread_start
	.type	__wasi_coop_thread_start,@function

__wasi_coop_thread_start:
	.functype	__wasi_coop_thread_start (i32) -> ()

	# Set up the minimum C environment.
	local.get   0  # start_arg
	i32.load    0  # stack
	call __wasm_component_model_builtin_context_set_0

	local.get   0  # start_arg
	i32.load    4  # tls_base
	call __wasm_component_model_builtin_context_set_1
	# Make the C function do the rest of work.
    call __wasm_component_model_builtin_thread_index
	local.get   0  # start_arg
	call __wasi_coop_thread_start_C

	end_function