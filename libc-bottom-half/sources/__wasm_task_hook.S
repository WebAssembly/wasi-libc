#include <wasi/version.h>

// The size, in bytes, of the stack statically allocated for
// `__wasilibc_task_hook`.  This doesn't need to be too big as it's just running
// `malloc` as defined in wasi-libc. The current number is determined by:
//
// * Start with 16 bytes.
// * Double until tests don't crash in a debug build.
// * Double again.
//
// The goal is to be pretty modest since this is part of every module.
#define HOOK_STACK_SIZE 1024

.text

.globl       __wasm_set_stack_pointer
.functype    __wasm_set_stack_pointer (i32) -> ()
.globl       __wasm_get_stack_pointer
.functype    __wasm_get_stack_pointer () -> (i32)
.functype    __wasilibc_task_hook (i32, i32, i32) -> (i32)

.globaltype  __memory_base, i32, immutable

// Hook point for component model task lifecycle events which is invoked by
// `wit-component`'s componentization process.
//
// Calls to this function are injected by `wit-component` when a component is
// created from a core module. This is invoked with single `i32` parameter
// indicating what's being hooked. Hooks include situations such as a
// synchronous task starting, an asynchronous task blocking, cabi_realloc
// returning, etc. The goal of this hook is to manage the `context.{get,set}`
// slots for this task, notably the stack pointer and TLS base pointer.
//
// When this function is invoked the context slots may both be 0, indicating
// that there is not a stack, hence the reason for this being an assembly
// function. This function quickly sets up a small temporary stack, allocated
// below as `hook_stack`, and then delegates to the `__wasilibc_task_hook`
// function which finishes the task started here. The return value of
// `__wasilibc_task_hook` becomes the stack pointer after this function returns.
.globl  __wasm_task_hook
.export_name  __wasm_task_hook, __wasm_task_hook
.type  __wasm_task_hook,@function
__wasm_task_hook:
  .functype  __wasm_task_hook (i32) -> ()
  .local i32

  // prep arguments to `__wasilibc_task_hook`, notably:
  //
  // * The hook being configured (this function's argument)
  // * The current stack pointer (if we're deallocating a stack)
  // * The hook stack pointer (passed further down below)
  local.get 0
  call __wasm_get_stack_pointer

  // setup a temporary stack
  i32.const hook_stack@MBREL
  global.get __memory_base
  i32.add
  i32.const HOOK_STACK_SIZE
  i32.add
  local.tee 1
  call __wasm_set_stack_pointer

  // figure out what our stack should be after this hook invocation and
  // configure it.
  local.get 1
  call __wasilibc_task_hook
  call __wasm_set_stack_pointer

  end_function

// Define a data symbol which is the stack that `__wasilibc_task_hook` runs on.
// Note that this is aligned to 16-bytes, the expected stack alignment.
.type hook_stack,@object
.section .bss.hook_stack,"",@
.p2align 4, 0x0
hook_stack:
  .skip  HOOK_STACK_SIZE
  .size hook_stack,  HOOK_STACK_SIZE
