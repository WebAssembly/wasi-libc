// Internal helpers for managing thread-local storage on the `wasm32-wasip3`
// target.
//
// See https://github.com/WebAssembly/wasi-libc/issues/857 for more info on the
// design.

#ifndef __WASI_WASIP3_TLS_H
#define __WASI_WASIP3_TLS_H

#include <stddef.h>
#include <wasi/wasip3.h>

#ifdef __wasm_libcall_thread_context__

// Rounds `value` up to a multiple of `align`, which must be a power of two.
static inline uintptr_t align_up(uintptr_t value, size_t align) {
  return (value + (align - 1)) & -(uintptr_t)align;
}

struct __wasilibc_library_tls_info {
  // Stores this library's TLS alignment in `*align` and returns its TLS size.
  //
  // Note that a library with no thread-local storage at all reports a size of
  // zero, but `*align` is always at least 1 so that callers can use it as a
  // divisor/mask without special-casing.
  size_t (*tls_size_and_align)(size_t *align);
  void (*init_tls)(void *);
};

struct __wasilibc_program_tls_info {
  size_t num_libraries;
  const struct __wasilibc_library_tls_info **library_info;
  void **main_thread_tls_base;
};

// Synthesized by `wit-component` for multi-module programs only, hence the weak
// declaration: a null address means this is a single-module program.
extern const struct __wasilibc_program_tls_info __wasm_program_tls_info
    __attribute__((__weak__));

// Returns the program's TLS description, or null if this is a single module.
static inline const struct __wasilibc_program_tls_info *
__wasilibc_program_tls_info(void) {
  return &__wasm_program_tls_info;
}

// Computes how much memory the thread-local storage of every library in this
// program requires, storing the required alignment in `*align`.
//
// For a single-module program this is just this module's own TLS.
size_t __wasilibc_tls_size(size_t *align);

// Installs `layout`, as produced by `__wasilibc_layout_thread_tls`, as the
// thread-local storage of whatever is currently running and initializes the
// contents of every library's block within it.
void __wasilibc_tls_init(void *layout);

extern void *__wasm_get_tls_base(void);
extern void __wasm_set_tls_base(void *base);

// Gets the TLS base pointer for the implicit main thread of the program. For
// dynamic linking scenarios this is provided in `__wasilibc_program_tls_info`
// and otherwise it's whatever's in `__init_tls_base`, initialized when our
// module was instantiated via the `wasm-ld`-injected `start` function.
static inline void *__wasilibc_tls_main_thread_base(void) {
  const struct __wasilibc_program_tls_info *info =
      __wasilibc_program_tls_info();
  if (info != NULL)
    return (void *)info->main_thread_tls_base;
  void *ret;
  __asm__(
#ifdef __PIC__
      ".globaltype __init_tls_base, i32\n"
#else
      ".globaltype __init_tls_base, i32, immutable\n"
#endif
      "global.get __init_tls_base\n"
      "local.set %0\n"
      : "=r"(ret));
  return ret;
}

#endif // __wasm_libcall_thread_context__

#endif // __WASI_WASIP3_TLS_H
