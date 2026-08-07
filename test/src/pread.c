#include "test.h"
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#define TEST(c)                                                                \
  do {                                                                         \
    errno = 0;                                                                 \
    if (!(c))                                                                  \
      t_error("%s failed (errno = %d)\n", #c, errno);                         \
  } while (0)

int main(void) {
  char tmp[] = "testsuite-pread";
  int fd;
  TEST((fd = open(tmp, O_RDWR | O_CREAT | O_EXCL, 0600)) > 2);
  TEST(write(fd, "Hello, world!", 13) == 13);

  // Reads at an offset return the requested bytes and leave the file
  // cursor untouched.
  char buf[8];
  TEST(pread(fd, buf, 5, 7) == 5);
  TEST(memcmp(buf, "world", 5) == 0);
  TEST(pread(fd, buf, 5, 0) == 5);
  TEST(memcmp(buf, "Hello", 5) == 0);
  TEST(lseek(fd, 0, SEEK_CUR) == 13);

  // Reads at and past EOF return 0.
  TEST(pread(fd, buf, 5, 13) == 0);
  TEST(pread(fd, buf, 5, 100) == 0);

  // A failing read must report the error: -1 with errno set, never
  // bytes. (A descriptor opened write-only cannot be read.)
  int wr_fd;
  TEST((wr_fd = open(tmp, O_WRONLY)) > 2);
  errno = 0;
  TEST(pread(wr_fd, buf, 5, 0) == -1);
  TEST(errno != 0);

  TEST(close(fd) == 0);
  TEST(close(wr_fd) == 0);
  TEST(unlink(tmp) == 0);
  return t_status;
}
