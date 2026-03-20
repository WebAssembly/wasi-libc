#include "test.h"
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wasi/libc.h>

#define TEST(c)                                                                \
  do {                                                                         \
    errno = 0;                                                                 \
    if (!(c))                                                                  \
      t_error("%s failed (errno = %d)\n", #c, errno);                          \
  } while (0)

int main(void) {
  char tmp[] = "testsuite-XXXXXX";
  int fd;
  TEST((fd = open(tmp, O_RDWR | O_CREAT | O_EXCL, 0600)) > 2);
  TEST(write(fd, "hello", 6) == 6);
  TEST(write(2, "test stderr\n", 12) == 12);
  // This should clear the entire descriptor table, closing all descriptors
  // (e.g. as the last step of a Wizer-style pre-init function).
  __wasilibc_reset_preopens();
  if (!(write(fd, "hello", 6) == -1 && errno == EBADF))
    t_error("fd should have been closed by `__wasilibc_reset_preopens`");
  // stdio handles will be lazily reinitialized, so should still work:
  TEST(write(2, "test stderr\n", 12) == 12);
  if (fd > 2)
    TEST(unlink(tmp) != -1);
  return t_status;
}
