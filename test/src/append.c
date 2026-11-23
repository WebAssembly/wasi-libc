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
  TEST(close(read_fd) == 0);

  // Toggling `O_APPEND` with `fcntl`
  TEST((fd = open(tmp, O_RDWR | O_TRUNC)) > 2);
  TEST(write(fd, "abcdefghijklmnop", 16) == 16);
  TEST(lseek(fd, 0, SEEK_SET) == 0);
  // Write `A` at the beginning (no append)
  TEST(write(fd, "A", 1) == 1);
  // Write `Z` at the end (with append)
  TEST(fcntl(fd, F_SETFL, O_APPEND) == 0);
  TEST((fcntl(fd, F_GETFL) & O_APPEND) != 0);
  TEST(write(fd, "Z", 1) == 1);
  // Write `Y` at the current position (without append), ends up at the end
  TEST(fcntl(fd, F_SETFL, 0) == 0);
  TEST((fcntl(fd, F_GETFL) & O_APPEND) == 0);
  TEST(write(fd, "Y", 1) == 1);
  // size check
  TEST(lseek(fd, 0, SEEK_CUR) == 18);
  // write `C` in the middle
  TEST(lseek(fd, 2, SEEK_SET) == 2);
  TEST(write(fd, "C", 1) == 1);
  // Frob the mode, then make sure `D` shows up after `C`
  TEST(fcntl(fd, F_SETFL, O_APPEND) == 0);
  TEST(fcntl(fd, F_SETFL, 0) == 0);
  TEST(write(fd, "D", 1) == 1);
  TEST(close(fd) == 0);

  TEST((read_fd = open(tmp, O_RDONLY)) > 2);
  memset(buf, 0, sizeof(buf));
  TEST(read(read_fd, buf, 30) == 18);
  TEST(memcmp(buf, "AbCDefghijklmnopZY", 18) == 0);
  TEST(close(read_fd) == 0);

  // In append mode the offset isn't moved until a write happens.
  TEST((fd = open(tmp, O_RDWR | O_APPEND)) > 2);
  TEST(lseek(fd, 0, SEEK_CUR) == 0);
  TEST(read(fd, buf, 2) == 2);
  TEST(memcmp(buf, "Ab", 2) == 0);
  TEST(lseek(fd, 0, SEEK_CUR) == 2);
  TEST(fcntl(fd, F_SETFL, 0) == 0);
  TEST(fcntl(fd, F_SETFL, O_APPEND) == 0);
  TEST(lseek(fd, 0, SEEK_CUR) == 2);

  // After an append-mode write the offset is at the end of the file, so a
  // subsequent read sees EOF.
  TEST(write(fd, "X", 1) == 1);
  TEST(read(fd, buf, sizeof(buf)) == 0);
  TEST(lseek(fd, 0, SEEK_CUR) == 19);
  TEST(lseek(fd, -3, SEEK_END) == 16);
  TEST(read(fd, buf, sizeof(buf)) == 3);
  TEST(memcmp(buf, "ZYX", 3) == 0);

  // Reading from the middle of the file, then appending, also moves the
  // offset to the end.
  TEST(lseek(fd, 0, SEEK_SET) == 0);
  TEST(read(fd, buf, 2) == 2);
  TEST(write(fd, "W", 1) == 1);
  TEST(read(fd, buf, sizeof(buf)) == 0);
  TEST(lseek(fd, 0, SEEK_CUR) == 20);
  TEST(close(fd) == 0);

  TEST(unlink(tmp) == 0);

  return t_status;
}
