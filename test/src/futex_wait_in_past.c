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
  int r;
  // A deadline in the past must report timeout when the value matches.
  struct timespec past;
  TEST(clock_gettime(CLOCK_REALTIME, &past) == 0);
  past.tv_nsec -= 1000;
  if (past.tv_nsec < 0) {
    past.tv_nsec += 1000000000L;
    past.tv_sec -= 1;
  }

  r = __wasilibc_futex_wait((volatile void *)&fut, 7, CLOCK_REALTIME, &past, 0);
  TEST(r == -ETIMEDOUT);

  return t_status;
}
