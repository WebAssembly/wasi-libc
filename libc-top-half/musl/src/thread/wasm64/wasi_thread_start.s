	.text

	.export_name	wasi_thread_start, wasi_thread_start

	.globaltype	__stack_pointer, i64
	.globaltype	__tls_base, i64
	.functype	__wasi_thread_start_C (i64, i64) -> ()

	.hidden	wasi_thread_start
	.globl	wasi_thread_start
	.type	wasi_thread_start,@function

wasi_thread_start:
	.functype	wasi_thread_start (i64, i64) -> ()

	# Set up the minimum C environment.
	# Note: offsetof(start_arg, stack) == 0
	local.get   1  # start_arg
	i64.load    0  # stack
	global.set  __stack_pointer

	local.get   1  # start_arg
	i64.load    8  # tls_base
	global.set  __tls_base

	# Make the C function do the rest of work.
	local.get   0  # tid
	local.get   1  # start_arg
	call __wasi_thread_start_C

	end_function
