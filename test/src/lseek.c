#include <sys/stat.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include "test.h"

#define TEST(c) do { \
	errno = 0; \
	if (!(c)) \
		t_error("%s failed (errno = %d)\n", #c, errno); \
} while(0)


int main(void) {
  int fd;
  TEST((fd = open("a.txt", O_RDWR | O_CREAT, 0644)) >= 0);

  TEST(lseek(fd, 0, SEEK_END) == 0);
  TEST(lseek(fd, 0, SEEK_SET) == 0);
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
  return t_status;
}
