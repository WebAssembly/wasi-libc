#include "test.h"
#include <errno.h>
#include <fcntl.h>
#include <string.h>
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

  TEST(fcntl(-1, F_DUPFD, 0) == -1 && errno == EBADF);
  TEST(fcntl(1234, F_DUPFD, 0) == -1 && errno == EBADF);

  int fd = open("dupfd.txt", O_RDWR | O_CREAT | O_EXCL, 0600);
  TEST(fd > 2);
  TEST(write(fd, "hello", 5) == 5);

  TEST(fcntl(fd, F_DUPFD, -1) == -1 && errno == EINVAL);
  TEST(fcntl(fd, F_DUPFD, 1 << 30) == -1 && errno == EINVAL);

  int d0 = fcntl(fd, F_DUPFD, 0);
  TEST(d0 > 2 && d0 != fd);

  TEST(lseek(d0, 0, SEEK_CUR) == 5);
  TEST(fcntl(d0, F_SETFL, O_NONBLOCK) == 0);
  TEST((fcntl(fd, F_GETFL) & O_NONBLOCK) != 0);
  TEST(fcntl(fd, F_SETFL, 0) == 0);

  int d1 = fcntl(fd, F_DUPFD, fd);
  TEST(d1 > fd && d1 != d0);

  TEST(close(d0) == 0);
  int minfd = fd + 10;
  int d2 = fcntl(fd, F_DUPFD, minfd);
  TEST(d2 >= minfd);

  int d3 = fcntl(fd, F_DUPFD_CLOEXEC, 0);
  TEST(d3 == d0);
  TEST(fcntl(d3, F_GETFD) == FD_CLOEXEC);
  TEST(lseek(d3, 0, SEEK_CUR) == 5);

  TEST(close(fd) == 0);
  char buf[8] = {0};
  TEST(lseek(d3, 0, SEEK_SET) == 0);
  TEST(read(d3, buf, sizeof(buf)) == 5);
  TEST(memcmp(buf, "hello", 5) == 0);

  TEST(close(d1) == 0);
  TEST(close(d2) == 0);

  int fds[32];
  for (int i = 0; i < 32; i++) {
    fds[i] = open("dupfd.txt", O_RDONLY);
    TEST(fds[i] > 2);
    TEST(fds[i] != d3);
    for (int j = 0; j < i; j++)
      TEST(fds[i] != fds[j]);
    TEST(read(fds[i], buf, 1) == 1);
    TEST(buf[0] == 'h');
  }
  for (int i = 0; i < 32; i++)
    TEST(close(fds[i]) == 0);

  TEST(close(d3) == 0);
  TEST(unlink("dupfd.txt") == 0);
  return t_status;
}
