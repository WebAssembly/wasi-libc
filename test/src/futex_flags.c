#include "test.h"
#include <errno.h>
#include <time.h>
#include <wasi/libc.h>

#define TEST(c)                                                                \
  do {                                                                         \
    if (!(c))                                                                  \
      t_error("%s failed\n", #c);                                              \
  } while (0)

int main(void) {
  volatile int fut = 7;

  // Passing bad flags must fail immediately with EINVAL.
  int r = __wasilibc_futex_wait((volatile void *)&fut, 7, CLOCK_REALTIME, NULL,
                                123);
  TEST(r == -EINVAL);

  r = __wasilibc_futex_wake((volatile void *)&fut, 1, 123);
  TEST(r == -EINVAL);

  return t_status;
}
