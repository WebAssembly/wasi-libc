#include "test.h"
#include <setjmp.h>
#include <stdlib.h>

#define TEST(c)                                                                \
  do {                                                                         \
    if (!(c))                                                                  \
      t_error("%s failed\n", #c);                                              \
  } while (0)

static void test_a() {
  jmp_buf env;
  int val = setjmp(env);
  if (val == 0) {
    longjmp(env, 42);
    abort();
  } else {
    TEST(val == 42);
  }
}

static jmp_buf b_env;

static void b_child2() {
  longjmp(b_env, 43);
  abort();
}

static void b_child() {
  b_child2();
  abort();
}

static void test_b() {
  int val = setjmp(b_env);
  if (val == 0) {
    b_child();
    abort();
  } else {
    TEST(val == 43);
  }
}

int main() {
  test_a();
  test_b();
  return t_status;
}
