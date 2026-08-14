#include "test.h"
#include <caller.h>
#include <string.h>

#define TEST(c)                                                                \
  do {                                                                         \
    if (!(c))                                                                  \
      t_error("%s failed\n", #c);                                              \
  } while (0)

int main() {
  caller_string_t arg, ret;
  arg.ptr = (uint8_t *)"hello";
  arg.len = 5;
  test_simple_i_hello(&arg, &ret);
  TEST(ret.len == 5);
  TEST(memcmp(ret.ptr, "hello", 5) == 0);
  return t_status + test_simple_i_t_status();
}
