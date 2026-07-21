#include "test.h"
#include <arpa/inet.h>
#include <errno.h>
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

static int listen_fd;

static void *poller(void *arg) {
  (void)arg;
  struct pollfd pfd = {.fd = listen_fd, .events = POLLRDNORM, .revents = 0};
  TEST(poll(&pfd, 1, -1) == 1);
  TEST(pfd.revents & POLLRDNORM);
  return NULL;
}

static void *acceptor(void *arg) {
  (void)arg;
  int fd = accept(listen_fd, NULL, NULL);
  TEST(fd >= 0);
  TEST(close(fd) == 0);
  return NULL;
}

int main(void) {
  listen_fd = socket(AF_INET, SOCK_STREAM, 0);
  TEST(listen_fd >= 0);

  struct sockaddr_in addr;
  socklen_t addr_len = sizeof(addr);
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  addr.sin_port = 0;
  TEST(bind(listen_fd, (struct sockaddr *)&addr, sizeof(addr)) == 0);
  TEST(getsockname(listen_fd, (struct sockaddr *)&addr, &addr_len) == 0);
  TEST(listen(listen_fd, 4) == 0);

  // Park a poller which owns the pending accept.
  pthread_t pt;
  TEST(pthread_create(&pt, NULL, poller, NULL) == 0);
  for (int i = 0; i < 10; i++)
    TEST(sched_yield() == 0);

  // Can't simultaneously poll right now.
  struct pollfd pfd = {.fd = listen_fd, .events = POLLRDNORM, .revents = 0};
  TEST(poll(&pfd, 1, 0) == -1 && errno == EOPNOTSUPP);

  // A blocking accept queues behind the poll.
  pthread_t at;
  TEST(pthread_create(&at, NULL, acceptor, NULL) == 0);
  for (int i = 0; i < 10; i++)
    TEST(sched_yield() == 0);

  // complete the two threads
  int cfd = socket(AF_INET, SOCK_STREAM, 0);
  TEST(cfd >= 0);
  TEST(connect(cfd, (struct sockaddr *)&addr, sizeof(addr)) == 0);
  TEST(pthread_join(pt, NULL) == 0);
  TEST(pthread_join(at, NULL) == 0);
  TEST(close(cfd) == 0);

  // The listening socket still works afterwards.
  int cfd2 = socket(AF_INET, SOCK_STREAM, 0);
  TEST(cfd2 >= 0);
  TEST(connect(cfd2, (struct sockaddr *)&addr, sizeof(addr)) == 0);
  int sfd = accept(listen_fd, NULL, NULL);
  TEST(sfd >= 0);

  TEST(close(sfd) == 0);
  TEST(close(cfd2) == 0);
  TEST(close(listen_fd) == 0);
  return t_status;
}
