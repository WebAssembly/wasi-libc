#include "test.h"
#include <errno.h>
#include <pthread.h>
#include <semaphore.h>
#include <string.h>

#ifndef __wasip1__
#define entrypoint exports_entrypoint_entrypoint
#endif

#define TEST(c)                                                                \
  do {                                                                         \
    if (!(c))                                                                  \
      t_error("%s failed\n", #c);                                              \
  } while (0)

static void dtor(void *ptr) {
  (void)ptr;
  __builtin_trap();
}

void entrypoint(void) {
  pthread_key_t a, b;
  TEST(pthread_key_create(&a, dtor) == 0);
  TEST(pthread_key_create(&b, dtor) == 0);
  TEST(pthread_key_delete(a) == 0);
  TEST(pthread_key_delete(b) == 0);
}
