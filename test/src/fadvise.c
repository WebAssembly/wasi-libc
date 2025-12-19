#include "test.h"
#include <errno.h>
#include <fcntl.h>
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

int main(void) {
  char tmp[] = "testsuite-XXXXXX";
  int fd;
  int flags = 0;

  TEST((fd = open(tmp, flags | O_WRONLY | O_CREAT | O_EXCL, 0600)) > 2);
  TEST(write(fd, "hello", 6) == 6);
  // Test each possible value
  TEST(posix_fadvise(fd, 1, 3, POSIX_FADV_NORMAL) == 0);
  TEST(posix_fadvise(fd, 2, 4, POSIX_FADV_SEQUENTIAL) == 0);
  TEST(posix_fadvise(fd, 3, 6, POSIX_FADV_RANDOM) == 0);
  TEST(posix_fadvise(fd, 0, 1, POSIX_FADV_NOREUSE) == 0);
  TEST(posix_fadvise(fd, 5, 6, POSIX_FADV_WILLNEED) == 0);
  TEST(posix_fadvise(fd, 0, 3, POSIX_FADV_DONTNEED) == 0);
  // Test bad file descriptor
  int badfd = fd + 1;
  TEST(posix_fadvise(badfd, 0, 0, POSIX_FADV_RANDOM) == EBADF);
  // Test out-of-bounds offset (not an error)
  TEST(posix_fadvise(fd, 42, 3, POSIX_FADV_NOREUSE) == 0);
  // Test in-bounds offset with out-of-bounds len (not an error)
  TEST(posix_fadvise(fd, 1, 42, POSIX_FADV_WILLNEED) == 0);
  close(fd);
  // Test negative offset and len (error)
  TEST(posix_fadvise(fd, -1, 0, POSIX_FADV_WILLNEED) == EINVAL);
  TEST(posix_fadvise(fd, 0, -2, POSIX_FADV_WILLNEED) == EINVAL);
  // Test that it fails on a directory
  TEST(mkdir("t", 0755) == 0);
  TEST((fd = open("t", flags | O_RDONLY) > 2));
  TEST(posix_fadvise(fd, 1, 42, POSIX_FADV_WILLNEED) == EBADF);

  close(fd);
  TEST(unlink(tmp) != -1);
  TEST(rmdir("t") != -1);

  return t_status;
}
