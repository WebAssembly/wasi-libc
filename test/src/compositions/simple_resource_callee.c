#include "test.h"
#include <callee.h>
#include <stdlib.h>

#define TEST(c)                                                                \
  do {                                                                         \
    if (!(c))                                                                  \
      t_error("%s failed\n", #c);                                              \
  } while (0)

struct exports_test_simple_i_r_t {
  uint32_t val;
};

void exports_test_simple_i_r_destructor(exports_test_simple_i_r_t *loc) {
  TEST(loc);
  free(loc);
}

exports_test_simple_i_own_r_t exports_test_simple_i_constructor_r(uint32_t x) {
  exports_test_simple_i_r_t *loc = malloc(sizeof(exports_test_simple_i_r_t));
  TEST(loc);
  loc->val = x;
  return exports_test_simple_i_r_new(loc);
}

uint32_t exports_test_simple_i_method_r_get(exports_test_simple_i_r_t *loc) {
  TEST(loc);
  return loc->val;
}

void exports_test_simple_i_method_r_set(exports_test_simple_i_r_t *loc,
                                        uint32_t x) {
  TEST(loc);
  loc->val = x;
}

uint32_t exports_test_simple_i_t_status() { return t_status; }
