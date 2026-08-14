// A header that's intended to only be `#include`'d in `crt-*.o` objects. This
// injects references to symbols for WASIp3 that are necessary to get linking
// to work.

#ifdef __wasm_libcall_thread_context__

#include <wasi/version.h>

// Force some symbols to be linked in for wasip3
extern void __wasm_task_hook(uint32_t);
extern void cabi_realloc(void);
__attribute__((used)) static void *__wasm_task_hook_ref = __wasm_task_hook;
__attribute__((used)) static void *cabi_realloc_ref = cabi_realloc;

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
