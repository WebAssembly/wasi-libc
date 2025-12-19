// Modified from libc-test to not use tmpfile or /dev/null

#include "test.h"
#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>

#define TEST(c, ...) ((c) ? 1 : (t_error(#c " failed: " __VA_ARGS__), 0))

int main(void) {
  char tmp[] = "testsuite-XXXXXX";
  int fd;
  int flags = 0;

  // Not a very useful test, but it's hard to test poll() without threads

  TEST((fd = open(tmp, flags | O_RDWR | O_CREAT | O_EXCL, 0600)) > 2);
  TEST(write(fd, "hello", 6) == 6);

  struct pollfd poll_fd = {.fd = fd, .events = POLLRDNORM, .revents = 0};
  int r = poll(&poll_fd, 1, -1);
  TEST(r == 1, "poll returned %d, expected 1\n", r);

  poll_fd.events = POLLWRNORM;
  r = poll(&poll_fd, 1, -1);
  TEST(r == 1, "poll returned %d, expected 1\n", r);

  close(fd);

  if (fd > 2)
    TEST(unlink(tmp) != -1);

  // skip negative fds
  poll_fd.fd = -300;
  r = poll(&poll_fd, 1, 0);
  TEST(r == 0, "poll returned %d, expected 0\n", r);

  // fail on invalid fds
  poll_fd.fd = 300;
  r = poll(&poll_fd, 1, 0);
  int err = errno;
  TEST(r == -1 && err == EBADF, "poll returned %d, expected -1\n", r);
  return t_status;
}
