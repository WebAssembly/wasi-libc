	.text

	.export_name	wasip3_thread_start, wasip3_thread_start

	.globaltype	__init_stack_pointer, i32
	.functype	__wasip3_thread_start_C (i32, i32) -> ()
	.functype   __wasm_component_model_builtin_context_set_0 (i32) -> ()
    .import_module __wasm_component_model_builtin_context_set_0, "$root"
    .import_name __wasm_component_model_builtin_context_set_0, "[context-set-0]"
    .functype   __wasm_component_model_builtin_context_set_1 (i32) -> ()
    .import_module __wasm_component_model_builtin_context_set_1, "$root"
    .import_name __wasm_component_model_builtin_context_set_1, "[context-set-1]"
	.import_module __wasm_component_model_builtin_thread_index, "$root"
	.import_name __wasm_component_model_builtin_thread_index, "[thread-index]"
    .functype   __wasm_component_model_builtin_thread_index () -> (i32)

	.hidden	wasip3_thread_start
	.globl	wasip3_thread_start
	.type	wasip3_thread_start,@function

wasip3_thread_start:
	.functype	wasip3_thread_start (i32) -> ()

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
	call __wasip3_thread_start_C

	# Unlock thread list. (as CLONE_CHILD_CLEARTID would do for Linux)
	#
	# Note: once we unlock the thread list, our "map_base" can be freed
	# by a joining thread. It's safe as we are in ASM and no longer use
	# our C stack or pthread_t. It's impossible to do this safely in C
	# because there is no way to tell the C compiler not to use C stack.
	i32.const   __thread_list_lock
	i32.const   0
	i32.atomic.store 0
	# As an optimization, we can check tl_lock_waiters here.
	# But for now, simply wake up unconditionally as
	# CLONE_CHILD_CLEARTID does.
	i32.const   __thread_list_lock
	i32.const   1
	memory.atomic.notify 0
	drop

	end_function