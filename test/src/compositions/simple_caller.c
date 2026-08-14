#include "test.h"
#include <caller.h>

#define TEST(c)                                                                \
  do {                                                                         \
    if (!(c))                                                                  \
      t_error("%s failed\n", #c);                                              \
  } while (0)

int main() {
  TEST(test_simple_i_hello() == 7);
  return t_status;
}
