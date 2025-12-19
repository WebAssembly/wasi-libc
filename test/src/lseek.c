#include "test.h"
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#define TEST(c)                                                                \
  do {                                                                         \
    errno = 0;                                                                 \
    if (!(c))                                                                  \
      t_error("%s failed (errno = %d)\n", #c, errno);                          \
  } while (0)

int main(void) {
  int fd;
  TEST((fd = open("a.txt", O_RDWR | O_CREAT, 0644)) >= 0);

  TEST(lseek(fd, 0, SEEK_END) == 0);
  TEST(lseek(fd, 0, SEEK_SET) == 0);
  TEST(lseek(fd, 0, SEEK_CUR) == 0);

  TEST(lseek(fd, -1, SEEK_END) == -1);
  TEST(lseek(fd, -1, SEEK_CUR) == -1);
  TEST(lseek(fd, -1, SEEK_SET) == -1);
  TEST(lseek(fd, 0, SEEK_CUR) == 0);

  TEST(write(fd, "Hello, World!", 13) == 13);
  TEST(lseek(fd, 0, SEEK_CUR) == 13);
  TEST(lseek(fd, 0, SEEK_SET) == 0);
  TEST(lseek(fd, -1, SEEK_END) == 12);

  char buf[10];
  TEST(lseek(fd, 1, SEEK_SET) == 1);
  TEST(read(fd, buf, 5) == 5);
  TEST(memcmp(buf, "ello,", 5) == 0);
  TEST(lseek(fd, 0, SEEK_CUR) == 6);

  TEST(close(fd) == 0);

  TEST((fd = open("b.txt", O_WRONLY | O_CREAT, 0644)) >= 0);
  char bytes[20] = {0};
  TEST(write(fd, bytes, 20) == 20);

  TEST(lseek(fd, 0, SEEK_CUR) == 20);

  TEST(lseek(fd, 0, SEEK_SET) == 0);
  TEST(lseek(fd, 0, SEEK_CUR) == 0);

  TEST(lseek(fd, 10, SEEK_SET) == 10);
  TEST(lseek(fd, 0, SEEK_CUR) == 10);

  TEST(lseek(fd, 5, SEEK_CUR) == 15);
  TEST(lseek(fd, 0, SEEK_CUR) == 15);

  TEST(lseek(fd, -5, SEEK_CUR) == 10);
  TEST(lseek(fd, 0, SEEK_CUR) == 10);

  TEST(lseek(fd, -5, SEEK_END) == 15);
  TEST(lseek(fd, 0, SEEK_CUR) == 15);

  TEST(close(fd) == 0);

  return t_status;
}
