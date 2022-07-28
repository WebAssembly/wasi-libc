uintptr_t __get_tp(void) {
#if _REENTRANT
  int val;
  __asm__(".globaltype g_needs_dynamic_alloc, i32\n"
          "global.get __wasilibc_pthread_self\n"
          "local.set %0" : "=r" (val));
  return val;
#else
  return 0;
#endif
}
