#include <wasi/version.h>

.text

  .globaltype  __init_stack_pointer, i32, immutable
  .functype   __wasm_set_stack_pointer (i32) -> ()
  .globl __wasm_set_stack_pointer

#ifdef __wasi_cooperative_threads__
#ifdef __PIC__
  .globaltype __init_tls_base, i32
#else
  .globaltype __init_tls_base, i32, immutable
#endif
  .functype   __wasm_set_tls_base (i32) -> ()
  .functype   __wasilibc_init_async_task () -> (i32)
  .functype   __wasi_init_tp () -> ()
#endif

  .globl  __wasm_init_task
  .export_name  __wasm_init_task, __wasm_init_task
  .type  __wasm_init_task,@function

  .globl  __wasm_init_async_task
  .type  __wasm_init_async_task,@function
  .export_name    __wasm_init_async_task, __wasm_init_async_task

__wasm_init_task:
  .functype  __wasm_init_task () -> ()

  // Synchronous tasks will return before anything else can enter this
  // component, so use the initial stack for this task.
  global.get   __init_stack_pointer
  call __wasm_set_stack_pointer

#ifdef __wasi_cooperative_threads__
  // For synchronous tasks in coop threads they inherit the same TLS as the
  // original "main thread" that started the module.
  global.get   __init_tls_base
  call __wasm_set_tls_base
#endif

  end_function

__wasm_init_async_task:
  .functype  __wasm_init_async_task () -> ()

  // Setup a temporary stack for this task to invoke `malloc` on. Note that if
  // threading is disabled then this is the actual stack of this task since no
  // other tasks can enter while this is blocked and/or actively running.
  global.get   __init_stack_pointer
  call __wasm_set_stack_pointer

#ifdef __wasi_cooperative_threads__
  // Setup the initial TLS from the "main thread" in case `malloc` uses TLS.
  // This won't be the TLS used by this task, however, in the long run.
  global.get   __init_tls_base
  call __wasm_set_tls_base

  // Defer to the C code in wasi-libc to finish initialization. This'll
  // allocate a new stack, setup this thread's TLS block, etc. The return
  // value of the function is what to use a the stack pointer for the rest of
  // this async task.
  call __wasilibc_init_async_task
  call __wasm_set_stack_pointer
#endif

  end_function
