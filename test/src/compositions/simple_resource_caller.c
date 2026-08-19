#include "test.h"
#include <caller.h>

#define TEST(c)                                                                \
  do {                                                                         \
    if (!(c))                                                                  \
      t_error("%s failed\n", #c);                                              \
  } while (0)

int main() {
  test_simple_i_own_r_t r = test_simple_i_constructor_r(10);
  test_simple_i_borrow_r_t r2 = test_simple_i_borrow_r(r);
  TEST(test_simple_i_method_r_get(r2) == 10);
  test_simple_i_method_r_set(r2, 12);
  TEST(test_simple_i_method_r_get(r2) == 12);
  test_simple_i_r_drop_own(r);
  return t_status + test_simple_i_t_status();
}
