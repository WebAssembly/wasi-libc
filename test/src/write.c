#include "test.h"
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <unistd.h>

#define TEST(c)                                                                \
  do {                                                                         \
    errno = 0;                                                                 \
    if (!(c))                                                                  \
      t_error("%s failed (errno = %d)\n", #c, errno);                          \
  } while (0)

#define N (100 << 20)

int main(void) {
  char tmp[] = "testsuite-XXXXXX";
  int fd;
  ssize_t r;

  TEST((fd = open(tmp, O_RDWR | O_CREAT | O_EXCL, 0600)) > 2);

  // write a big file of zeros, looping until it's fully written.
  char *bytes = calloc(1, N);
  TEST(bytes != NULL);
  size_t written = 0;
  while (written < N) {
    TEST((r = write(fd, bytes + written, N - written)) > 0);
    written += r;
  }
  TEST(close(fd) == 0);

  struct stat buf;
  TEST(stat(tmp, &buf) == 0);
  TEST(buf.st_size == N);

  return t_status;
}
