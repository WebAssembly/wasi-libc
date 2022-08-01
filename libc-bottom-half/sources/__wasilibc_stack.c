#include <sys/types.h>

void* __wasilibc_get_stack_pointer(void) {
  void* val;
#if defined(__wasm64__)
  __asm__(".globaltype __stack_pointer, i64\n"
          "global.get __stack_pointer\n"
          "local.set %0" : "=r" (val));
#else
  __asm__(".globaltype __stack_pointer, i32\n"
          "global.get __stack_pointer\n"
          "local.set %0" : "=r" (val));
#endif
  return val;
}

void __wasilibc_set_stack_pointer(void *val) {
#if defined(__wasm64__)
  __asm__(".globaltype __stack_pointer, i64\n"
          "local.get %0\n"
          "global.set __stack_pointer" : /* no outputs */ : "r" (val));
#else
  __asm__(".globaltype __stack_pointer, i32\n"
          "local.get %0\n"
          "global.set __stack_pointer" : /* no outputs */ : "r" (val));
#endif
}

void __wasm_init_tls(size_t val);

void __wasilibc_init_tls(void *val) {
  __wasm_init_tls((size_t)val);
}

unsigned long long __wasilibc_tls_size(void) {
  size_t val;
#if defined(__wasm64__)
  __asm__(".globaltype __tls_size, i64, immutable\n"
          "global.get __tls_size\n"
          "local.set %0" : "=r" (val));
#else
  __asm__(".globaltype __tls_size, i32, immutable\n"
          "global.get __tls_size\n"
          "local.set %0" : "=r" (val));
#endif
  return (unsigned long long)val;
}

unsigned long long __wasilibc_tls_align(void) {
  size_t val;
#if defined(__wasm64__)
  __asm__(".globaltype __tls_align, i64, immutable\n"
          "global.get __tls_align\n"
          "local.set %0" : "=r" (val));
#else
  __asm__(".globaltype __tls_align, i32, immutable\n"
          "global.get __tls_align\n"
          "local.set %0" : "=r" (val));
#endif
  return (unsigned long long)val;
}

void* __wasilibc_get_tls_base(void) {
  void* val;
#if defined(__wasm64__)
  __asm__(".globaltype __tls_base, i64\n"
          "global.get __tls_base\n"
          "local.set %0" : "=r" (val));
#else
  __asm__(".globaltype __tls_base, i32\n"
          "global.get __tls_base\n"
          "local.set %0" : "=r" (val));
#endif
  return val;
}

void __wasilibc_set_tls_base(void *val) {
#if defined(__wasm64__)
  __asm__(".globaltype __tls_base, i64\n"
          "local.get %0\n"
          "global.set __tls_base" : /* no outputs */ : "r" (val));
#else
  __asm__(".globaltype __tls_base, i32\n"
          "local.get %0\n"
          "global.set __tls_base" : /* no outputs */ : "r" (val));
#endif
}
