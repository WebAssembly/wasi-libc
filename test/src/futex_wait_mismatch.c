#include "test.h"
#include <errno.h>
#include <time.h>
#include <wasi/libc.h>

#define TEST(c)                                                                \
  do {                                                                         \
    if (!(c))                                                                  \
      t_error("%s failed\n", #c);                                            \
  } while (0)

int main(void)
{
  volatile int fut = 7;

  // A mismatched expected value must fail immediately.
  int r = __wasilibc_futex_wait((volatile void *)&fut, 8, CLOCK_REALTIME, NULL, 0);
  TEST(r == -EWOULDBLOCK);

  return t_status;
}
