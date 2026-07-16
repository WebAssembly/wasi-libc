#include "test.h"
#include <errno.h>
#include <poll.h>
#include <string.h>
#include <unistd.h>

#define TEST(c)                                                                \
  do {                                                                         \
    errno = 0;                                                                 \
    if (!(c))                                                                  \
      t_error("%s failed (errno = %d)\n", #c, errno);                          \
  } while (0)

int main(void) {
  int bad_fd = dup(1);
  TEST(bad_fd >= 0);
  TEST(close(bad_fd) == 0);

  struct pollfd pfd = {.fd = bad_fd, .events = POLLIN, .revents = 0};
  TEST(poll(&pfd, 1, 0) == 1);
  TEST(pfd.revents == POLLNVAL);

  struct pollfd pfds[2] = {
      {.fd = bad_fd, .events = POLLIN, .revents = 0},
      {.fd = 1, .events = POLLOUT, .revents = 0},
  };
  TEST(poll(pfds, 2, -1) == 2);
  TEST(pfds[0].revents == POLLNVAL);
  TEST(pfds[1].revents & POLLOUT);

  struct pollfd neg = {.fd = -1, .events = POLLIN, .revents = 0};
  TEST(poll(&neg, 1, 0) == 0);
  TEST(neg.revents == 0);

  return t_status;
}
