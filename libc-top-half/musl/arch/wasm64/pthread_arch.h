static inline uintptr_t __get_tp(void) {
#if _REENTRANT
  int val;
  __asm__("global.get __wasilibc_pthread_self\n"
          "local.set %0"
          : "=r"(val));
  return val;
#else
  return 0;
#endif
}
