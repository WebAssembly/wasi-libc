#include "test.h"
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>

#define TEST(c)                                                                \
  do {                                                                         \
    errno = 0;                                                                 \
    if (!(c))                                                                  \
      t_error("%s failed (errno = %d)\n", #c, errno);                          \
  } while (0)

int main(void) {
  char tmp[] = "testsuite-XXXXXX";
  int fd;
  int nbytes = 0;
  int flags = 0;
  flags |= O_RDWR | O_CREAT | O_EXCL;

  TEST((fd = open(tmp, flags, 0600)) > 2);
  TEST(write(fd, "hello", 6) == 6);

#ifdef __wasilibc_use_wasip2
  // Not supported on wasip2
  TEST(ioctl(fd, FIONREAD, &nbytes) == -1);
#else
  // Always returns 0?
  TEST(ioctl(fd, FIONREAD, &nbytes) == 0);
#endif
  close(fd);
  TEST(unlink(tmp) == 0);

  return t_status;
}
