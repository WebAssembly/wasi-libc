#ifdef __wasm_libcall_thread_context__

#include <wasi/version.h>
#include <wasi/wasip3_tls.h>

#ifdef __wasi_cooperative_threads__

void __wasilibc_set_task_tls(void *init_tls_base) {
  const struct __wasilibc_program_tls_info *info =
      __wasilibc_program_tls_info();

  // With a single module in the component the TLS base is stored directly in
  // context slot 1, so this module's own initial TLS is what belongs there.
  // With more than one module the slot instead holds the array of per-module
  // TLS base pointers, and the main thread's array was populated by each module
  // as the component was instantiated.
  wasip3_context_set_1(info == NULL ? init_tls_base
                                    : (void *)info->main_thread_tls_base);
}

#endif // __wasi_cooperative_threads__

#endif // __wasm_libcall_thread_context__
