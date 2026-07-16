#include "test.h"
#include <errno.h>
#include <fcntl.h>
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
#ifdef __wasip1__
  if (1)
    return t_status;
#endif

  TEST(dup(-1) == -1 && errno == EBADF);
  TEST(dup(1234) == -1 && errno == EBADF);

  int fd = open("dup-file.txt", O_RDWR | O_CREAT | O_EXCL, 0600);
  TEST(fd > 2);
  int fd2 = dup(fd);
  TEST(fd2 >= 0);
  TEST(fd2 != fd);

  TEST(write(fd, "hello ", 6) == 6);
  TEST(lseek(fd2, 0, SEEK_CUR) == 6);
  TEST(write(fd2, "world", 5) == 5);
  TEST(lseek(fd, 0, SEEK_CUR) == 11);

  TEST(close(fd) == 0);
  TEST(write(fd2, "!", 1) == 1);

  TEST(lseek(fd2, 0, SEEK_SET) == 0);
  char buf[16] = {0};
  TEST(read(fd2, buf, sizeof(buf)) == 12);
  TEST(memcmp(buf, "hello world!", 12) == 0);

  int fd3 = dup(fd2);
  TEST(fd3 >= 0);
  TEST(fcntl(fd2, F_SETFL, O_NONBLOCK) == 0);
  TEST((fcntl(fd3, F_GETFL) & O_NONBLOCK) != 0);
  TEST(close(fd3) == 0);

  int fd4 = fcntl(fd2, F_DUPFD, 10);
  TEST(fd4 >= 10);
  TEST(lseek(fd4, 0, SEEK_END) == 12);
  TEST(close(fd4) == 0);

  TEST(close(fd2) == 0);
  TEST(unlink("dup-file.txt") == 0);
  return t_status;
}
