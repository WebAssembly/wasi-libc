#include "test.h"
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <poll.h>
#include <pthread.h>
#include <sched.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define TEST(c)                                                                \
  do {                                                                         \
    errno = 0;                                                                 \
    if (!(c))                                                                  \
      t_error("%s failed (errno = %d)\n", #c, errno);                          \
  } while (0)

int main(void) {
  int lfd = socket(AF_INET, SOCK_STREAM, 0);
  TEST(lfd >= 0);
  struct sockaddr_in addr;
  socklen_t addr_len = sizeof(addr);
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  addr.sin_port = 0;
  TEST(bind(lfd, (struct sockaddr *)&addr, sizeof(addr)) == 0);
  TEST(getsockname(lfd, (struct sockaddr *)&addr, &addr_len) == 0);
  TEST(listen(lfd, 1) == 0);

  int cfd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
  TEST(cfd >= 0);
  errno = 0;
  int rc = connect(cfd, (struct sockaddr *)&addr, sizeof(addr));
  int connect_errno = errno;
  if (rc == 0)
    return t_status;
  if (!(rc == -1 && connect_errno == EINPROGRESS))
    t_error("connect failed (rc = %d, errno = %d)\n", rc, connect_errno);

  // Polling the connecting socket twice in one call isn't supported.
  struct pollfd pfds[2] = {
      {.fd = cfd, .events = POLLWRNORM, .revents = 0},
      {.fd = cfd, .events = POLLWRNORM, .revents = 0},
  };
  TEST(poll(pfds, 2, -1) == -1 && errno == EOPNOTSUPP);

  // The failed poll cleaned up after itself: a single-entry poll completes the
  // connect.
  struct pollfd one = {.fd = cfd, .events = POLLWRNORM, .revents = 0};
  TEST(poll(&one, 1, -1) == 1);
  TEST(one.revents & POLLWRNORM);

  // And the connect actually succeeded.
  int err = -1;
  socklen_t err_len = sizeof(err);
  TEST(getsockopt(cfd, SOL_SOCKET, SO_ERROR, &err, &err_len) == 0);
  TEST(err == 0);

  TEST(close(cfd) == 0);
  TEST(close(lfd) == 0);
  return t_status;
}
