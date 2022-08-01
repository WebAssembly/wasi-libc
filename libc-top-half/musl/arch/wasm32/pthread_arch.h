static inline uintptr_t __get_tp(void) {
  uintptr_t val;
  __asm__(".globaltype __tls_base, i32\n"
          "global.get __tls_base\n"
          "local.set %0" : "=r" (val));
  return val + 64;
}