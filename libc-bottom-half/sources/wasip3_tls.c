#include <wasi/wasip3_tls.h>

size_t __wasilibc_tls_size(size_t *align) {
  const struct __wasilibc_program_tls_info *info =
      __wasilibc_program_tls_info();
  if (info == NULL) {
    *align = __builtin_wasm_tls_align();
    return __builtin_wasm_tls_size();
  }

  // The array of per-library TLS base pointers goes first, followed by each
  // library's own block at its required alignment.
  uintptr_t size = info->num_libraries * sizeof(void *);
  size_t result_align = _Alignof(void *);

  for (size_t i = 0; i < info->num_libraries; i++) {
    const struct __wasilibc_library_tls_info *library = info->library_info[i];
    size_t library_align;
    size_t library_size = library->tls_size_and_align(&library_align);
    if (library_size == 0)
      continue;
    size = align_up(size, library_align) + library_size;
    if (library_align > result_align)
      result_align = library_align;
  }

  *align = result_align;
  return align_up(size, result_align);
}

// Synthesized by `wasm-ld` for this module. Points this module's TLS base at
// `base` and then initializes the contents of the block located there.
extern void __wasm_init_tls(void *base);

void __wasilibc_tls_init(void *layout) {
  const struct __wasilibc_program_tls_info *info =
      __wasilibc_program_tls_info();

  // For a single-module program `__wasm_init_tls` both installs `layout` as
  // the current TLS base and initializes its contents.
  if (info == NULL) {
    __wasm_init_tls(layout);
    return;
  }

  // For a multi-module program the TLS base pointer each library's imported
  // `__wasm_{get,set}_tls_base` functions are distinctly synthesized by
  // `wit-component` to unique functions. Each function loads from a static
  // offset from a base pointer, and the base pointer is itself static when coop
  // threads are disabled or it's context slot 1 with coop threads.
  //
  // This calculates the appropriate base pointer for each library and invoke's
  // the library's `__wasm_init_tls` function, which like above is synthesized
  // by `wasm-ld`. This function will then write the per-library pointer to the
  // base-pointer-array internally, so that's not done here.
#ifdef __wasi_cooperative_threads__
  wasip3_context_set_1(layout);
#endif
  uintptr_t next = (uintptr_t)layout + info->num_libraries * sizeof(void *);
  for (size_t i = 0; i < info->num_libraries; i++) {
    const struct __wasilibc_library_tls_info *library = info->library_info[i];
    size_t library_align;
    size_t library_size = library->tls_size_and_align(&library_align);
    if (library_size == 0)
      continue;
    next = align_up(next, library_align);
    info->library_info[i]->init_tls((void *)next);
    next += library_size;
  }
}
