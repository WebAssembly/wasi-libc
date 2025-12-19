#include "test.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/times.h>
#include <time.h>

#define TEST(c, ...) ((c) ? 1 : (t_error(#c " failed: " __VA_ARGS__), 0))

int main(void) {
  time_t t;
  struct tms buffer;

  clock_t result = times(&buffer);
  TEST(result > 0);
  TEST(buffer.tms_utime > 0);
  TEST(buffer.tms_cutime == 0); // always 0 under WASI

  time_t t1 = time(&t);
  TEST(t == t1);
  TEST(t > 0);

  return t_status;
}
