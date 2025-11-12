#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include "test.h"

#define TEST(c) do { \
	errno = 0; \
	if (!(c)) \
		t_error("%s failed (errno = %d)\n", #c, errno); \
} while(0)

int main(void) {
  char tmp[] = "testsuite-XXXXXX";
  int fd;
  TEST((fd = open(tmp, O_RDWR | O_CREAT | O_EXCL | O_APPEND, 0600)) > 2);

  TEST(write(fd, "Hello, world!", 13) == 13);
  TEST(lseek(fd, 0, SEEK_CUR) == 13);
  TEST(lseek(fd, 0, SEEK_SET) == 0);
  TEST(write(fd, "Hello, world!", 13) == 13);
  TEST(lseek(fd, 0, SEEK_CUR) == 26);
  TEST(lseek(fd, 0, SEEK_SET) == 0);

  TEST(close(fd) == 0);

  int read_fd;
  TEST((read_fd = open(tmp, O_RDONLY)) > 2);

  char buf[30];
  TEST(read(read_fd, buf, 30) == 26);
  TEST(memcmp(buf, "Hello, world!Hello, world!", 26) == 0);

  TEST((fd = open(tmp, O_RDWR | O_APPEND, 0600)) > 2);
  TEST(write(fd, "hi", 2) == 2);
  TEST(close(fd) == 0);

  TEST(lseek(read_fd, 0, SEEK_SET) == 0);
  TEST(read(read_fd, buf, 30) == 28);
  TEST(memcmp(buf, "Hello, world!Hello, world!hi", 28) == 0);

  return t_status;
}
