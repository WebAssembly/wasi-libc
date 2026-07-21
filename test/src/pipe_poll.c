#include "test.h"
#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <string.h>
#include <unistd.h>

#define TEST(c)                                                                \
  do {                                                                         \
    errno = 0;                                                                 \
    if (!(c))                                                                  \
      t_error("%s failed (errno = %d)\n", #c, errno);                          \
  } while (0)

static char big[8192];

static size_t fill_pipe(int wr) {
  memset(big, 'a', sizeof(big));
  size_t total = 0;
  for (int i = 0; i < 1000; i++) {
    ssize_t n = write(wr, big, sizeof(big));
    if (n == -1) {
      int err = errno;
      TEST(err == EAGAIN);
      break;
    }
    total += n;
  }
  return total;
}

int main(void) {
#if defined(__wasip1__) || defined(__wasip2__)
  if (1)
    return t_status;
#endif

  int fds[2];
  TEST(pipe2(fds, O_NONBLOCK) == 0);

  // Empty pipe: read end is not ready, write end is.
  struct pollfd pfd = {.fd = fds[0], .events = POLLIN, .revents = 0};
  TEST(poll(&pfd, 1, 0) == 0);
  pfd = (struct pollfd){.fd = fds[1], .events = POLLOUT, .revents = 0};
  TEST(poll(&pfd, 1, 0) == 1);
  TEST(pfd.revents & POLLOUT);

  // Timing out exercises the register-then-cancel path on the read end.
  pfd = (struct pollfd){.fd = fds[0], .events = POLLIN, .revents = 0};
  TEST(poll(&pfd, 1, 50) == 0);

  // The pipe still works after a cancelled poll registration.
  TEST(write(fds[1], "x", 1) == 1);
  pfd = (struct pollfd){.fd = fds[0], .events = POLLIN, .revents = 0};
  TEST(poll(&pfd, 1, -1) == 1);
  TEST(pfd.revents & POLLIN);
  char buf[4];
  TEST(read(fds[0], buf, sizeof(buf)) == 1);
  TEST(buf[0] == 'x');

  // Polling both ends of the same pipe in one call.
  struct pollfd pfds[2] = {
      {.fd = fds[0], .events = POLLIN, .revents = 0},
      {.fd = fds[1], .events = POLLOUT, .revents = 0},
  };
  TEST(poll(pfds, 2, 0) == 1);
  TEST(pfds[0].revents == 0);
  TEST(pfds[1].revents & POLLOUT);

  // A full pipe is not ready for writing; timing out exercises the
  // register-then-cancel path on the write end.
  size_t total = fill_pipe(fds[1]);
  TEST(total > 0);
  pfd = (struct pollfd){.fd = fds[1], .events = POLLOUT, .revents = 0};
  TEST(poll(&pfd, 1, 0) == 0);
  TEST(poll(&pfd, 1, 50) == 0);

  // Draining restores write readiness, and the write end still works after
  // its cancelled poll registration.
  size_t drained = 0;
  while (drained < total) {
    ssize_t n = read(fds[0], big, sizeof(big));
    TEST(n > 0);
    if (n <= 0)
      break;
    drained += n;
  }
  TEST(drained == total);
  pfd = (struct pollfd){.fd = fds[1], .events = POLLOUT, .revents = 0};
  TEST(poll(&pfd, 1, 0) == 1);
  TEST(pfd.revents & POLLOUT);
  TEST(write(fds[1], "y", 1) == 1);
  TEST(read(fds[0], buf, sizeof(buf)) == 1);
  TEST(buf[0] == 'y');

  // Closing the write end makes the read end ready (EOF).
  TEST(close(fds[1]) == 0);
  pfd = (struct pollfd){.fd = fds[0], .events = POLLIN, .revents = 0};
  TEST(poll(&pfd, 1, -1) == 1);
  TEST(pfd.revents & (POLLIN | POLLHUP));
  TEST(read(fds[0], buf, sizeof(buf)) == 0);
  TEST(close(fds[0]) == 0);

  // Closing the read end makes the write end ready (write returns EPIPE).
  TEST(pipe(fds) == 0);
  TEST(close(fds[0]) == 0);
  pfd = (struct pollfd){.fd = fds[1], .events = POLLOUT, .revents = 0};
  TEST(poll(&pfd, 1, -1) == 1);
  TEST(pfd.revents & (POLLOUT | POLLERR));
  TEST(close(fds[1]) == 0);

  return t_status;
}
