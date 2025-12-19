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
  struct stat st;
  int fd;
  int flags = 0;
  flags |= O_RDWR | O_CREAT | O_EXCL;

  TEST((fd = open(tmp, flags, 0200)) > 2);
  TEST(write(fd, "hello world", 11) == 11);
  // Truncate to a smaller size
  TEST(ftruncate(fd, 5) == 0);
  close(fd);

  TEST(stat(tmp, &st) == 0);
  TEST(st.st_size == 5);

  TEST((fd = open(tmp, O_RDWR, 0200)) > 2);
  // Truncate to a larger size
  TEST(ftruncate(fd, 11) == 0);
  // length < 0 is an error
  TEST(ftruncate(fd, -1) == -1);
  close(fd);

  TEST(stat(tmp, &st) == 0);
  TEST(st.st_size == 11);

  // Nonexistent file descriptor is an error
  int badfd = fd + 1;
  TEST(ftruncate(badfd, 1) == -1);

  TEST(unlink(tmp) != -1);

  return t_status;
}
