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

int fd_size(const char *path) {
  struct stat st;
  stat(path, &st);
  return st.st_size;
}

int main(void) {
  char tmp[] = "testsuite-XXXXXX";
  int fd;
  int flags = 0;

  TEST((fd = open(tmp, flags | O_WRONLY | O_CREAT | O_EXCL, 0600)) > 2);
  TEST(write(fd, "hello", 6) == 6);

  // This operation is not supported
  TEST(posix_fallocate(fd, 6, 5) == ENOTSUP);

  close(fd);
  TEST(unlink(tmp) != -1);

  return t_status;
}
