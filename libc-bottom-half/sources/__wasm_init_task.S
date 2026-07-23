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
#endif


// Entrypoint for initializing both async and sync tasks in the component model.
//
// Calls to this function are injected by `wit-component` when a component is
// created from a core module. Specifically for lifted functions this
// initialization hook is invoked first before the user-specified lifted
// function. The purpose of this function is to configure this task's stack/TLS
// setup and its `context.{get,set}` slots.
//
// For both sync and async tasks once they're entered in this component it's
// guaranteed that no other task will start before they have returned. That
// means that it's safe to use the same stack for all tasks. This does mean that
// the same TLS values are used across all tasks, which seem reasonable-enough
// for now.
//
// Note that configuring TLS is only required in coop threads builds since when
// coop-threads are disabled TLS is routed through a global in wasi-libc which
// needs no extra handling (unlike coop threads where TLS routes through
// `context.{get,set}` which needs setting up).
.globl  __wasm_init_task
.export_name  __wasm_init_task, __wasm_init_task
.type  __wasm_init_task,@function
__wasm_init_task:
  .functype  __wasm_init_task () -> ()

  global.get   __init_stack_pointer
  call __wasm_set_stack_pointer

#ifdef __wasi_cooperative_threads__
  global.get   __init_tls_base
  call __wasm_set_tls_base
#endif
  end_function

// This is just a small wrapper around `__wasm_init_task`. Work needs to be done
// in `wit-component` to make this optional.
.globl  __wasm_init_async_task
.type  __wasm_init_async_task,@function
.export_name    __wasm_init_async_task, __wasm_init_async_task
__wasm_init_async_task:
  .functype  __wasm_init_async_task () -> ()
  call __wasm_init_task
  end_function
