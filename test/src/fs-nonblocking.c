#include "test.h"
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <stdbool.h>
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

int main() {
  int fd;
  TEST((fd = open("howdy.txt", O_RDWR | O_NONBLOCK | O_CREAT, 0755)) > 0);

  int rc, size = 0;

  struct pollfd pollfd;
  pollfd.fd = fd;

  pollfd.events = POLLWRNORM;
  for (int i = 0; i < 100; i++) {
    fprintf(stderr, "iteration %d size: %d\n", i, size);
    while ((rc = write(fd, "hello", 5)) > 0)
      size += rc;
    TEST(poll(&pollfd, 1, -1) != -1);
  }

  TEST(size > 0);

  struct stat stat;
  TEST(fstat(fd, &stat) == 0);
  TEST(stat.st_size == size);

  TEST(lseek(fd, 0, SEEK_SET) != -1);

  char buf[5];
  pollfd.events = POLLRDNORM;
  int remaining = size;
  while (remaining) {
    while ((rc = read(fd, buf, sizeof(buf))) > 0)
      remaining -= rc;
    TEST(poll(&pollfd, 1, -1) != -1);
  }

  TEST(close(fd) != -1);
  return t_status;
}
