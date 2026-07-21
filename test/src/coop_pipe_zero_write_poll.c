#include "test.h"
#include <errno.h>
#include <poll.h>
#include <pthread.h>
#include <sched.h>
#include <stdatomic.h>
#include <unistd.h>

#define TEST(c)                                                                \
  do {                                                                         \
    errno = 0;                                                                 \
    if (!(c))                                                                  \
      t_error("%s failed (errno = %d)\n", #c, errno);                          \
  } while (0)

static int fds[2];
static _Atomic int poll_done;

static void *poller(void *arg) {
  (void)arg;
  struct pollfd pfd = {.fd = fds[0], .events = POLLIN, .revents = 0};
  TEST(poll(&pfd, 1, -1) == 1);
  poll_done = 1;
  TEST(pfd.revents & POLLIN);
  return NULL;
}

int main(void) {
  TEST(pipe(fds) == 0);

  pthread_t t;
  TEST(pthread_create(&t, NULL, poller, NULL) == 0);
  for (int i = 0; i < 10; i++)
    TEST(sched_yield() == 0);

  // A zero-length write must not wake the poller...
  TEST(write(fds[1], "", 0) == 0);
  for (int i = 0; i < 10; i++)
    TEST(sched_yield() == 0);
  TEST(poll_done == 0);

  // ...but a real write must.
  TEST(write(fds[1], "x", 1) == 1);
  TEST(pthread_join(t, NULL) == 0);
  TEST(poll_done == 1);

  char c;
  TEST(read(fds[0], &c, 1) == 1);
  TEST(close(fds[0]) == 0);
  TEST(close(fds[1]) == 0);
  return t_status;
}
