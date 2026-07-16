  .text

  .export_name  __wasi_coop_thread_start, __wasi_coop_thread_start

  .functype   __wasi_coop_thread_start_C (i32) -> ()
  .functype   __wasm_init_tls (i32) -> ()
  .functype   __wasm_set_stack_pointer (i32) -> ()

  .hidden  __wasi_coop_thread_start
  .globl  __wasi_coop_thread_start
  .type  __wasi_coop_thread_start,@function

__wasi_coop_thread_start:
  .functype  __wasi_coop_thread_start (i32) -> ()

  # Configure the linear memory stack, as set up by the spawning thread.
  local.get   0  # start_arg
  i32.load    0  # stack
  call __wasm_set_stack_pointer

  # Now w
  local.get   0  # start_arg
  call __wasi_coop_thread_start_C

  end_function
