// This is a "surgical" augmentation for the generated `wasip{2,3}.c` files in
// wasi-libc which changes the behavior of the generated `cabi_realloc`. Ideally
// `wit-bindgen` would have a mode that doesn't generate `cabi_realloc`, but that
// doesn't exist right now so we're left to augment it.
//
// Specifically the change made here is that when
// `__wasm_libcall_thread_context__` is enabled, such as on wasip3 targets,
// this will insert a small inline-assembly wrapper function aruond the
// `cabi_realloc` C symbol. The purpose of this wrapper function is to
// configure the stack used during `cabi_realloc`. As specified in
// WebAssembly/component-model#680 the `context.{get,set}` slots are 0
// when `cabi_realloc` is called. This means that, by default, `cabi_realloc`
// has no stack to execute on. To solve this problem a smal lstack is injected
// here which is used exclusively for `cabi_realloc`. This is done because
// the main stack can't be used as its current depth can't be found (it's in
// some other component model task's context storage).
//
// To make bindings generation a bit easier this file is `#include`'d in a a
// single location hence the trailing `__attribute__` at the end which is
// applicable when `__wasm_libcall_thread_context__` is disabled.
//
// Note that it's somewhat important that this lives in the same file as
// `wasip3.c`'s translation unit because that way once bindings are used it forces
// `wasm-ld` to look at this file and see the `cabi_realloc_export` function
// and export it. Otherwise nothing refers to it so it wouldn't be exported.

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
#define REALLOC_STACK_SIZE "512"

__asm__(
    // Declare the `__wasm_{g,s}et_stack_pointer` functions.
    ".functype   __wasm_set_stack_pointer (i32) -> ()\n"
    ".functype   __wasm_get_stack_pointer () -> (i32)\n"

    // Go ahead and generate an `-fPIC`-compatible addressing mode for
    // `cabi_realloc_stack` so this is compatible with both PIC and not builds.
    ".globaltype  __memory_base, i32, immutable\n"

    // For coop-threads builds we need to configure TLS to the initial TLS
    // value for all tasks/etc, so import those symbols here.
#ifdef __wasi_cooperative_threads__
    ".functype   __wasm_set_tls_base (i32) -> ()\n"
    ".functype   __wasm_get_tls_base () -> (i32)\n"
#ifdef __PIC__
    ".globaltype __init_tls_base, i32\n"
#else
    ".globaltype __init_tls_base, i32, immutable\n"
#endif
#endif

    // Configure a symbol named `cabi_realloc_export` which is exported under
    // the core module name "cabi_realloc".
    ".export_name cabi_realloc_export, \"cabi_realloc\"\n"
    ".globl cabi_realloc_export\n"
    "cabi_realloc_export:\n"
    ".functype cabi_realloc_export (i32, i32, i32, i32) -> (i32)\n"

    // Prior to bytecodealliance/wasmtime#13949 wasmtime's handling of
    // `cabi_realloc` and context slots was a bit buggy. That means that prior
    // to that PR the slots aren't guaranteed to be 0 and the value present will
    // be used by some future task. This export is going to clobber these
    // slots, so while that Wasmtime change is percolating this preserves the
    // slots around the invocation of `cabi_realloc`. Once #13949 percolates and
    // ships then this workaround code, and the restore down below, can be
    // deleted.
    "  .local i32, i32\n"
    "  call __wasm_get_stack_pointer\n"
    "  local.set 4\n"
#ifdef __wasi_cooperative_threads__
    "  call __wasm_get_tls_base\n"
    "  local.set 5\n"
#endif

    // First up, configure the stack pointer. Get the base of the stack from the
    // data symbol defined below, add the stack size, and that's what we're
    // starting with.
    "  i32.const cabi_realloc_stack@MBREL\n"
    "  global.get __memory_base\n"
    "  i32.add\n"
    "  i32.const " REALLOC_STACK_SIZE "\n"
    "  i32.add\n"
    "  call __wasm_set_stack_pointer\n"

    // Next up configure TLS. This is only required in coop-threads builds.
#ifdef __wasi_cooperative_threads__
    "  global.get   __init_tls_base\n"
    "  call __wasm_set_tls_base\n"
#endif

    // And finally forward to the actual implementation of `cabi_realloc`
    // defined in `wasip3.c`
    "  local.get 0\n"
    "  local.get 1\n"
    "  local.get 2\n"
    "  local.get 3\n"
    "  call cabi_realloc\n"

    "  local.get 4\n"
    "  call __wasm_set_stack_pointer\n"
#ifdef __wasi_cooperative_threads__
    "  local.get 5\n"
    "  call __wasm_set_tls_base\n"
#endif

    "  end_function\n"

    // Define a data symbol which is the stack that `cabi_realloc` runs on.
    // Note that this is aligned to 16-bytes, the expected stack alignment.
    ".type cabi_realloc_stack,@object\n"
    ".section .bss.cabi_realloc_stack,\"\",@\n"
    ".p2align 4, 0x0\n"
    "cabi_realloc_stack:\n"
    "  .skip " REALLOC_STACK_SIZE "\n"
    "  .size cabi_realloc_stack, " REALLOC_STACK_SIZE "\n"
    "\n"
);
#else
__attribute__((__weak__, __export_name__("cabi_realloc")))
#endif
