#include "test.h"
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define TEST(c)                                                                \
  do {                                                                         \
    errno = 0;                                                                 \
    if (!(c))                                                                  \
      t_error("%s failed (errno = %d)\n", #c, errno);                          \
  } while (0)

int main(void) {
  char tmp[] = "testsuite-XXXXXX";
  char foo[6];
  int fd;
  int flags = 0;
  FILE *f;

  TEST((fd = open(tmp, flags | O_WRONLY | O_CREAT | O_EXCL, 0600)) > 2);
  TEST(write(fd, "hello", 6) == 6);
  TEST(read(fd, foo, 6) == -1);
  errno = 0;
  close(fd);

  TEST(f = fdopen(fd, "r"));
  if (f) {
    TEST(write(fd, "hello", 6) == -1);
    errno = 0;
    close(fd);
  }
  if (fd > 2)
    TEST(unlink(tmp) != -1);
  return t_status;
}
