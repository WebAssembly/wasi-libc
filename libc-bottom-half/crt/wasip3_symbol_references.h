// A header that's intended to only be `#include`'d in `crt-*.o` objects. This
// injects references to symbols for WASIp3 that are necessary to get linking
// to work.

#ifdef __wasm_libcall_thread_context__

#include <wasi/version.h>
#include <wasi/wasip3_tls.h>

// Force some symbols to be linked in for wasip3
extern void __wasm_init_task(void);
extern void __wasm_init_async_task(void);
extern void cabi_realloc(void);
__attribute__((used)) static void *__wasm_init_task_ref = __wasm_init_task;
__attribute__((used)) static void *__wasm_init_async_task_ref =
    __wasm_init_async_task;
__attribute__((used)) static void *cabi_realloc_ref = cabi_realloc;

// Export a symbol from this object which describes the TLS information
// required for this object. This encapsulates the size/align information as
// well as an initialization function.
//
// Note that being part of `crt1-*.o` this is exported from all libraries,
// which is what we want because each library has a unique return value from
// `__builtin_wasm_tls_*` intrinsics and a separate `__wasm_init_tls` function.
//
// For more information on this see
// https://github.com/WebAssembly/wasi-libc/issues/857 which has since been
// adjusted with this scheme.
static size_t tls_size_and_align(size_t *align) {
  *align = __builtin_wasm_tls_align();
  return __builtin_wasm_tls_size();
}

void __wasm_init_tls(void *base);

__attribute__((visibility("default")))
struct __wasilibc_library_tls_info __wasm_library_tls_info = {
  .tls_size_and_align = tls_size_and_align,
  .init_tls = __wasm_init_tls,
};

// Force `__wasm_{g,s}et_{stack_pointer,tls_base}` to exist as defined symbols.
// These end up as imported functions which `wit-component` recognizes, and what
// exactly they're hooked up to will depend on `wit-component` when this is
// turned into a component.
__asm__(
".globl      __wasm_get_stack_pointer\n"
".type       __wasm_get_stack_pointer,@function\n"
".functype __wasm_get_stack_pointer () -> (i32)\n"
".import_module __wasm_get_stack_pointer, \"env\"\n"
".import_name __wasm_get_stack_pointer, \"__wasm_get_stack_pointer\"\n"

".globl      __wasm_set_stack_pointer\n"
".type       __wasm_set_stack_pointer,@function\n"
".functype __wasm_set_stack_pointer (i32) -> ()\n"
".import_module __wasm_set_stack_pointer, \"env\"\n"
".import_name __wasm_set_stack_pointer, \"__wasm_set_stack_pointer\"\n"

".globl      __wasm_get_tls_base\n"
".type       __wasm_get_tls_base,@function\n"
".functype __wasm_get_tls_base () -> (i32)\n"
".import_module __wasm_get_tls_base, \"env\"\n"
".import_name __wasm_get_tls_base, \"__wasm_get_tls_base\"\n"

".globl      __wasm_set_tls_base\n"
".type       __wasm_set_tls_base,@function\n"
".functype __wasm_set_tls_base (i32) -> ()\n"
".import_module __wasm_set_tls_base, \"env\"\n"
".import_name __wasm_set_tls_base, \"__wasm_set_tls_base\"\n"

);

#endif
