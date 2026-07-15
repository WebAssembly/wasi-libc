#include "test.h"
#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <pthread.h>
#include <sched.h>
#include <string.h>
#include <unistd.h>

#define TEST(c)                                                                \
  do {                                                                         \
    errno = 0;                                                                 \
    if (!(c))                                                                  \
      t_error("%s failed (errno = %d)\n", #c, errno);                          \
  } while (0)

static int fds[2];

static void *poller(void *arg) {
  (void)arg;
  struct pollfd pfd = {.fd = fds[1], .events = POLLOUT, .revents = 0};
  TEST(poll(&pfd, 1, -1) == 1);
  TEST(pfd.revents & POLLOUT);
  TEST(write(fds[1], "y", 1) == 1);
  return NULL;
}

int main(void) {
  TEST(pipe(fds) == 0);

  // Fill the pipe, probing capacity with O_NONBLOCK.
  TEST(fcntl(fds[1], F_SETFL, O_NONBLOCK) == 0);
  static char big[8192];
  memset(big, 'a', sizeof(big));
  size_t total = 0;
  for (int i = 0; i < 1000; i++) {
    ssize_t n = write(fds[1], big, sizeof(big));
    if (n == -1) {
      int err = errno;
      TEST(err == EAGAIN);
      break;
    }
    TEST(n > 0);
    total += n;
  }
  TEST(total > 0);
  TEST(fcntl(fds[1], F_SETFL, 0) == 0);

  pthread_t t;
  TEST(pthread_create(&t, NULL, poller, NULL) == 0);
  for (int i = 0; i < 10; i++)
    TEST(sched_yield() == 0);

  // Make space; this should wake the poller with POLLOUT.
  char buf[256];
  TEST(read(fds[0], buf, sizeof(buf)) == (ssize_t)sizeof(buf));
  TEST(pthread_join(t, NULL) == 0);

  // Drain everything, including the poller's byte at the end.
  size_t drained = sizeof(buf);
  while (drained < total) {
    ssize_t n = read(fds[0], buf, sizeof(buf));
    TEST(n > 0);
    drained += n;
  }
  TEST(read(fds[0], buf, sizeof(buf)) == 1);
  TEST(buf[0] == 'y');

  TEST(close(fds[0]) == 0);
  TEST(close(fds[1]) == 0);
  return t_status;
}
