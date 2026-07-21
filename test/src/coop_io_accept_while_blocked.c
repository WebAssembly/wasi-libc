#include "test.h"
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <poll.h>
#include <pthread.h>
#include <sched.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define TEST(c)                                                                \
  do {                                                                         \
    errno = 0;                                                                 \
    if (!(c))                                                                  \
      t_error("%s failed (errno = %d)\n", #c, errno);                          \
  } while (0)

static int lfd;

static void *acceptor(void *arg) {
  (void)arg;
  int fd = accept(lfd, NULL, NULL);
  TEST(fd >= 0);
  TEST(close(fd) == 0);
  return NULL;
}

int main(void) {
  lfd = socket(AF_INET, SOCK_STREAM, 0);
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

  pthread_t at;
  TEST(pthread_create(&at, NULL, acceptor, NULL) == 0);

  // Make sure our child thread had a chance to get blocked in `accept`.
  for (int i = 0; i < 10; i++)
    TEST(sched_yield() == 0);

  // Concurrent nonblocking accepts should be reported as not supported.
  TEST(fcntl(lfd, F_SETFL, O_NONBLOCK) == 0);
  TEST(accept(lfd, NULL, NULL) == -1 && errno == EOPNOTSUPP);
  TEST(fcntl(lfd, F_SETFL, 0) == 0);

  // Unblock our child thread.
  int cfd = socket(AF_INET, SOCK_STREAM, 0);
  TEST(cfd >= 0);
  TEST(connect(cfd, (struct sockaddr *)&addr, sizeof(addr)) == 0);

  TEST(pthread_join(at, NULL) == 0);
  TEST(close(cfd) == 0);
  TEST(close(lfd) == 0);
  return t_status;
}
