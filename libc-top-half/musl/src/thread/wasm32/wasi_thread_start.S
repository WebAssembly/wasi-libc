#define PTR i32
#define PTRSIZE 4
# Preprocessor's define directive doesn't resolve math expressions,
# so we hardcode the result of PTRSIZE * 2 operation
#define DOUBLE_PTRSIZE 8

	.text

	.export_name	wasi_thread_start, wasi_thread_start

	.globaltype	__stack_pointer, PTR
	.globaltype	__tls_base, PTR

#ifndef NDEBUG
	.globaltype	__stack_base, PTR
	.globaltype	__stack_limit, PTR
#endif

	.functype	__wasi_thread_start_C (i32, PTR) -> ()

	.hidden	wasi_thread_start
	.globl	wasi_thread_start
	.type	wasi_thread_start,@function

wasi_thread_start:
	.functype	wasi_thread_start (i32, PTR) -> ()

	# Set up the minimum C environment.
	# Note: offsetof(start_arg, stack) == 0
	local.get   1  # start_arg
	PTR.load    0  # stack
	global.set  __stack_pointer

	local.get   1        # start_arg
	PTR.load    PTRSIZE  # tls_base
	global.set  __tls_base

#ifndef NDEBUG
	# configure __stack_base and __stack_limit in debug mode
	# to allow for stack overflow detection
	local.get   1  # start_arg
	PTR.load    0  # stack
	global.set  __stack_base

	local.get   1               # start_arg
	PTR.load    DOUBLE_PTRSIZE  # stack_limit
	global.set __stack_limit
#endif

	# Make the C function do the rest of work.
	local.get   0  # tid
	local.get   1  # start_arg
	call __wasi_thread_start_C

	end_function

#ifndef NDEBUG
.section .data,"",@

__stack_base:

__stack_limit:

#endif
