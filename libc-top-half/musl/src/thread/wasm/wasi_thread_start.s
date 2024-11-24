#ifdef __wasm64__
#define PTR i64
#define PTRSIZE 8
#else
#define PTR i32
#define PTRSIZE 4
#endif

	.text

	.export_name	wasi_thread_start, wasi_thread_start

	.globaltype	__stack_pointer, PTR
	.globaltype	__tls_base, PTR
	.functype	__wasi_thread_start_C (PTR, PTR) -> ()

	.hidden	wasi_thread_start
	.globl	wasi_thread_start
	.type	wasi_thread_start,@function

wasi_thread_start:
	.functype	wasi_thread_start (PTR, PTR) -> ()

	# Set up the minimum C environment.
	# Note: offsetof(start_arg, stack) == 0
	local.get   1  # start_arg
	PTR.load    0  # stack
	global.set  __stack_pointer

	local.get   1  # start_arg
	PTR.load    PTRSIZE  # tls_base
	global.set  __tls_base

	# Make the C function do the rest of work.
	local.get   0  # tid
	local.get   1  # start_arg
	call __wasi_thread_start_C

	# Unlock thread list. (as CLONE_CHILD_CLEARTID would do for Linux)
	#
	# Note: once we unlock the thread list, our "map_base" can be freed
	# by a joining thread. It's safe as we are in ASM and no longer use
	# our C stack or pthread_t. It's impossible to do this safely in C
	# because there is no way to tell the C compiler not to use C stack.
	PTR.const   __thread_list_lock
	PTR.const   0
	PTR.atomic.store 0
	# As an optimization, we can check tl_lock_waiters here.
	# But for now, simply wake up unconditionally as
	# CLONE_CHILD_CLEARTID does.
	PTR.const   __thread_list_lock
	i32.const   1
	memory.atomic.notify 0
	drop

	end_function
