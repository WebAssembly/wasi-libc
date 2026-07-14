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

static void tcp_loopback_pair(int *server_conn, int *client) {
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

  int cfd = socket(AF_INET, SOCK_STREAM, 0);
  TEST(cfd >= 0);
  TEST(connect(cfd, (struct sockaddr *)&addr, sizeof(addr)) == 0);

  int sfd = accept(lfd, NULL, NULL);
  TEST(sfd >= 0);

  TEST(close(lfd) == 0);
  *server_conn = sfd;
  *client = cfd;
}

static int conn_fd;

static void *poller(void *arg) {
  (void)arg;
  struct pollfd pfd = {.fd = conn_fd, .events = POLLRDNORM, .revents = 0};
  TEST(poll(&pfd, 1, -1) == 1);
  TEST(pfd.revents & POLLRDNORM);
  return NULL;
}

int main(void) {
  int client;
  tcp_loopback_pair(&conn_fd, &client);

  pthread_t pt;
  TEST(pthread_create(&pt, NULL, poller, NULL) == 0);
  for (int i = 0; i < 10; i++)
    TEST(sched_yield() == 0);

  // Can't shutdown the side being polled, but can shutdown the other.
  TEST(shutdown(conn_fd, SHUT_RD) == -1 && errno == EOPNOTSUPP);
  TEST(shutdown(conn_fd, SHUT_RDWR) == -1 && errno == EOPNOTSUPP);
  TEST(shutdown(conn_fd, SHUT_WR) == 0);

  // Complete the poll and drain the data.
  TEST(write(client, "ping", 4) == 4);
  TEST(pthread_join(pt, NULL) == 0);
  char buf[16];
  TEST(read(conn_fd, buf, sizeof(buf)) == 4);
  TEST(memcmp(buf, "ping", 4) == 0);

  // Now the read half can be shut down too.
  TEST(shutdown(conn_fd, SHUT_RD) == 0);
  TEST(read(conn_fd, buf, sizeof(buf)) == 0);

  TEST(close(client) == 0);
  TEST(close(conn_fd) == 0);
  return t_status;
}
