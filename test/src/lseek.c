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

  // Seeking to the end of the file after a write must actually move the
  // write cursor, even when the offset passed to `lseek` is 0.
  TEST((fd = open("c.txt", O_RDWR | O_CREAT | O_TRUNC, 0644)) >= 0);
  TEST(write(fd, "abcdefghijklmnop", 16) == 16);
  TEST(lseek(fd, 0, SEEK_SET) == 0);
  TEST(write(fd, "Q", 1) == 1);
  TEST(lseek(fd, 0, SEEK_END) == 16);
  TEST(write(fd, "Z", 1) == 1);
  TEST(lseek(fd, 0, SEEK_CUR) == 17);
  TEST(lseek(fd, 0, SEEK_SET) == 0);
  char contents[18] = {0};
  TEST(read(fd, contents, 17) == 17);
  TEST(memcmp(contents, "QbcdefghijklmnopZ", 17) == 0);

  // Same as above, but with a read stream that needs to be repositioned.
  TEST(lseek(fd, 0, SEEK_SET) == 0);
  TEST(read(fd, buf, 1) == 1);
  TEST(buf[0] == 'Q');
  TEST(lseek(fd, -1, SEEK_END) == 16);
  TEST(read(fd, buf, 1) == 1);
  TEST(buf[0] == 'Z');
  TEST(close(fd) == 0);

  // Same as above, but in append mode a read stream at the previous position
  // must not be reused after seeking to the end.
  TEST((fd = open("c.txt", O_RDWR | O_APPEND)) >= 0);
  TEST(read(fd, buf, 1) == 1);
  TEST(buf[0] == 'Q');
  TEST(lseek(fd, 0, SEEK_END) == 17);
  TEST(read(fd, buf, 1) == 0);
  TEST(lseek(fd, -2, SEEK_END) == 15);
  TEST(read(fd, buf, 2) == 2);
  TEST(memcmp(buf, "pZ", 2) == 0);
  TEST(close(fd) == 0);

  return t_status;
}
