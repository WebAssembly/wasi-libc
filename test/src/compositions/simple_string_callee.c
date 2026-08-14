#include "test.h"
#include <callee.h>
#include <string.h>

#define TEST(c)                                                                \
  do {                                                                         \
    if (!(c))                                                                  \
      t_error("%s failed\n", #c);                                              \
  } while (0)

void exports_test_simple_i_hello(callee_string_t *x, callee_string_t *ret) {
  TEST(x->len == 5);
  TEST(memcmp(x->ptr, "hello", 5) == 0);
  *ret = *x;
}

uint32_t exports_test_simple_i_t_status() { return t_status; }
