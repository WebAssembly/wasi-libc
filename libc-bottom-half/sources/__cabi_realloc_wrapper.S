#include <wasi/version.h>

#ifdef __wasm_libcall_thread_context__
// The size, in bytes, of the stack statically allocated for `cabi_realloc`.
// This doesn't need to be too big as it's just running `cabi_realloc` as defined
// in wasi-libc. The current number is determined by:
//
// * Start with 16 bytes.
// * Double until tests don't crash in a debug build.
// * Double again.
//
// The goal is to be pretty modest since this is part of every module.
#define REALLOC_STACK_SIZE 512

// Declare the `__wasm_{g,s}et_stack_pointer` functions.
.functype   __wasm_set_stack_pointer (i32) -> ()
.functype   __wasm_get_stack_pointer () -> (i32)
.type       __wasm_set_stack_pointer,@function
.type       __wasm_get_stack_pointer,@function
.functype   __cabi_realloc_impl (i32, i32, i32, i32) -> (i32)
.type       __cabi_realloc_impl,@function

// Go ahead and generate an `-fPIC`-compatible addressing mode for
// `cabi_realloc_stack` so this is compatible with both PIC and not builds.
.type        __memory_base,@global
.globaltype  __memory_base, i32, immutable

// For coop-threads builds we need to configure TLS to the initial TLS
// value for all tasks/etc, so import those symbols here.
#ifdef __wasi_cooperative_threads__
.functype   __wasm_set_tls_base (i32) -> ()
.functype   __wasm_get_tls_base () -> (i32)
.type       __wasm_set_tls_base,@function
.type       __wasm_get_tls_base,@function
.type       __init_tls_base,@global
#ifdef __PIC__
.globaltype __init_tls_base, i32
#else
.globaltype __init_tls_base, i32, immutable
#endif
#endif

// This wrapper takes over the `cabi_realloc` symbol itself and the
// `cabi_realloc` core module export. The C implementation it forwards to is
// renamed to `__cabi_realloc_impl` by `cabi_realloc_augment.h`.
//
// Note that it's important that the symbol name matches the export name here.
// In a `BUILD_SHARED=ON` build `libc.so` is dynamically linked against by other
// shared libraries and by `crt1-{command,reactor}.o`, and the name a symbol is
// resolved by in that context is its wasm export name. If `.export_name` were
// used to rename the export then this function would be unresolvable by name
// from anything linking against `libc.so`.
.globl cabi_realloc
.export_name cabi_realloc, "cabi_realloc"
.type cabi_realloc,@function
cabi_realloc:
  .functype cabi_realloc (i32, i32, i32, i32) -> (i32)

  // Prior to bytecodealliance/wasmtime#13949 wasmtime's handling of
  // `cabi_realloc` and context slots was a bit buggy. That means that prior
  // to that PR the slots aren't guaranteed to be 0 and the value present will
  // be used by some future task. This export is going to clobber these
  // slots, so while that Wasmtime change is percolating this preserves the
  // slots around the invocation of `cabi_realloc`. Once #13949 percolates and
  // ships then this workaround code, and the restore down below, can be
  // deleted.
  .local i32, i32
  call __wasm_get_stack_pointer
  local.set 4
#ifdef __wasi_cooperative_threads__
  call __wasm_get_tls_base
  local.set 5
#endif

  // First up, configure the stack pointer. Get the base of the stack from the
  // data symbol defined below, add the stack size, and that's what we're
  // starting with.
  i32.const cabi_realloc_stack@MBREL
  global.get __memory_base
  i32.add
  i32.const  REALLOC_STACK_SIZE
  i32.add
  call __wasm_set_stack_pointer

  // Next up configure TLS. This is only required in coop-threads builds.
#ifdef __wasi_cooperative_threads__
  global.get   __init_tls_base
  call __wasm_set_tls_base
#endif

  // And finally forward to the actual implementation of `cabi_realloc`
  // defined in `wasip3.c`
  local.get 0
  local.get 1
  local.get 2
  local.get 3
  call __cabi_realloc_impl

  local.get 4
  call __wasm_set_stack_pointer
#ifdef __wasi_cooperative_threads__
  local.get 5
  call __wasm_set_tls_base
#endif

  end_function

// Define a data symbol which is the stack that `cabi_realloc` runs on.
// Note that this is aligned to 16-bytes, the expected stack alignment.
.type cabi_realloc_stack,@object
.section .bss.cabi_realloc_stack,"",@
.p2align 4, 0x0
cabi_realloc_stack:
  .skip  REALLOC_STACK_SIZE
  .size cabi_realloc_stack,  REALLOC_STACK_SIZE

#endif // __wasm_libcall_thread_context__
