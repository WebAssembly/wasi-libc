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

#ifdef __wasi_cooperative_threads__

// Sets up context slot 1 for the currently running task. Invoked from inline
// assembly from wrappers of exported functions plus the cabi_realloc export wrapper.
void __wasilibc_set_task_tls(void *init_tls_base);

#endif // __wasi_cooperative_threads__

#endif // __wasm_libcall_thread_context__

#endif // __WASI_WASIP3_TLS_H
