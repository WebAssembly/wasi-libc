#include "test.h"
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define TEST(c)                                                                \
  do {                                                                         \
    errno = 0;                                                                 \
    if (!(c))                                                                  \
      t_error("%s failed (errno = %d)\n", #c, errno);                          \
  } while (0)

int main(void) {
  char tmp[] = "testsuite-XXXXXX";
  char tmp1[] = "testsuite-link";
  int fd;
  int flags = 0;

  TEST((fd = open(tmp, flags | O_WRONLY | O_CREAT | O_EXCL, 0600)) > 2);
  TEST(write(fd, "hello", 6) == 6);
  close(fd);

  TEST(link(tmp, tmp1) == 0);
  TEST(unlink(tmp1) != -1);

  TEST(linkat(AT_FDCWD, tmp, AT_FDCWD, tmp1, 0) == 0);
  TEST(unlink(tmp1) != -1);

  TEST(unlink(tmp) != -1);

  char src[] = "/dir/a";
  char dst[] = "/dir/b";

  TEST(mkdir("dir", 0700) == 0);
  TEST((fd = open(src, flags | O_WRONLY | O_CREAT | O_EXCL, 0600)) > 2);
  close(fd);

  TEST(link(src, dst) == 0);
  TEST(unlink(dst) != -1);

  TEST(linkat(AT_FDCWD, src, AT_FDCWD, dst, 0) == 0);
  TEST(unlink(dst) != -1);

  TEST(chdir("/dir") == 0);

  TEST(linkat(AT_FDCWD, "a", AT_FDCWD, "b", 0) == 0);
  TEST(unlink("b") != -1);

  TEST(chdir("/") == 0);

  TEST(unlink(src) != -1);

  return t_status;
}
