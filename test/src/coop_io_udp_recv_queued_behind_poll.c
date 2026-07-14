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

static int rx_fd;

static void *poller(void *arg) {
  (void)arg;
  struct pollfd pfd = {.fd = rx_fd, .events = POLLRDNORM, .revents = 0};
  TEST(poll(&pfd, 1, -1) == 1);
  TEST(pfd.revents & POLLRDNORM);
  return NULL;
}

static void *receiver(void *arg) {
  (void)arg;
  char buf[16];
  TEST(recvfrom(rx_fd, buf, sizeof(buf), 0, NULL, NULL) == 4);
  TEST(memcmp(buf, "ping", 4) == 0);
  return NULL;
}

int main(void) {
  rx_fd = socket(AF_INET, SOCK_DGRAM, 0);
  TEST(rx_fd >= 0);

  struct sockaddr_in addr;
  socklen_t addr_len = sizeof(addr);
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  addr.sin_port = 0;
  TEST(bind(rx_fd, (struct sockaddr *)&addr, sizeof(addr)) == 0);
  TEST(getsockname(rx_fd, (struct sockaddr *)&addr, &addr_len) == 0);

  // Thread blocked in polling.
  pthread_t pt;
  TEST(pthread_create(&pt, NULL, poller, NULL) == 0);
  for (int i = 0; i < 10; i++)
    TEST(sched_yield() == 0);

  // Can't do a concurrent nonblocking read.
  char buf[16];
  TEST(recvfrom(rx_fd, buf, sizeof(buf), MSG_DONTWAIT, NULL, NULL) == -1 &&
       errno == EOPNOTSUPP);

  // Also can't do a concurrent connect.
  struct sockaddr_in other = addr;
  TEST(connect(rx_fd, (struct sockaddr *)&other, sizeof(other)) == -1 &&
       errno == EOPNOTSUPP);

  // Can queue up other blocking work though.
  pthread_t rt;
  TEST(pthread_create(&rt, NULL, receiver, NULL) == 0);
  for (int i = 0; i < 10; i++)
    TEST(sched_yield() == 0);

  // Send a packet; the poll completes, then the queued recv gets the payload.
  int tx_fd = socket(AF_INET, SOCK_DGRAM, 0);
  TEST(tx_fd >= 0);
  TEST(sendto(tx_fd, "ping", 4, 0, (struct sockaddr *)&addr, sizeof(addr)) ==
       4);
  TEST(pthread_join(pt, NULL) == 0);
  TEST(pthread_join(rt, NULL) == 0);

  // The socket is fully usable afterwards, including `connect`.
  TEST(sendto(tx_fd, "more", 4, 0, (struct sockaddr *)&addr, sizeof(addr)) ==
       4);
  TEST(recvfrom(rx_fd, buf, sizeof(buf), 0, NULL, NULL) == 4);
  TEST(memcmp(buf, "more", 4) == 0);

  TEST(close(tx_fd) == 0);
  TEST(close(rx_fd) == 0);
  return t_status;
}
