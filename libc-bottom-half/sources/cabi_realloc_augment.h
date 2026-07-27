// This is a "surgical" augmentation for the generated `wasip{2,3}.c` files in
// wasi-libc which changes the behavior of the generated `cabi_realloc`. Ideally
// `wit-bindgen` would have a mode that doesn't generate `cabi_realloc`, but that
// doesn't exist right now so we're left to augment it.
//
// Specifically the change made here is that when
// `__wasm_libcall_thread_context__` is enabled, such as on wasip3 targets, the
// actual `cabi_realloc` entrypoint lives in an extern assembly file and the
// `cabi_realloc` symbol in the `wasip3.c` file is renamed to something else.
// This handles how `cabi_realloc` starts with context slots 0'd out and with
// a libcall thread context that means the stack needs to be configured.
//
// To make bindings generation a bit easier this file is `#include`'d in a a
// single location hence the trailing `__attribute__` at the end which is
// applicable when `__wasm_libcall_thread_context__` is disabled.

#ifdef __wasm_libcall_thread_context__
#define cabi_realloc __cabi_realloc_impl
#else
__attribute__((__weak__, __export_name__("cabi_realloc")))
#endif
