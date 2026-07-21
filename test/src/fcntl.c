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
  flags |= O_WRONLY | O_CREAT | O_EXCL;

  TEST((fd = open(tmp, flags, 0600)) > 2);

  int returned_flags = fcntl(fd, F_GETFL);
  TEST(returned_flags == O_WRONLY);

  // This can only change some GNU-specific flags,
  // so for now, just test that setting the same flags
  // doesn't change anything
  TEST(fcntl(fd, F_SETFL, returned_flags) == 0);
  returned_flags = fcntl(fd, F_GETFL);
  TEST(returned_flags == O_WRONLY);

  // Test with a nonexistent file descriptor
  int badfd = fd + 1;
  TEST(fcntl(badfd, F_GETFL) == -1);
  TEST(fcntl(badfd, F_SETFL, returned_flags) == -1);

  close(fd);

  // Test with a closed file descriptor
  TEST(fcntl(fd, F_GETFL) == -1);

  // file locking is unsupported
  {
    int fd2;
    TEST((fd2 = open(tmp, O_RDWR)) > 2);
    struct flock fl = {
        .l_type = F_WRLCK,
        .l_whence = SEEK_SET,
        .l_start = 0,
        .l_len = 0,
    };
    errno = 0;
    TEST(fcntl(fd2, F_SETLK, &fl) == -1 && errno == ENOTSUP);
    errno = 0;
    TEST(fcntl(fd2, F_SETLKW, &fl) == -1 && errno == ENOTSUP);
    errno = 0;
    TEST(fcntl(fd2, F_GETLK, &fl) == -1 && errno == ENOTSUP);
    close(fd2);
  }

  TEST(unlink(tmp) != -1);

  return t_status;
}
