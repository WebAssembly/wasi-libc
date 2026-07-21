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

  // Park one thread in an indefinite poll for readability.
  pthread_t pt;
  TEST(pthread_create(&pt, NULL, poller, NULL) == 0);
  for (int i = 0; i < 10; i++)
    TEST(sched_yield() == 0);

  // Can't concurrently poll
  struct pollfd pfd = {.fd = conn_fd, .events = POLLRDNORM, .revents = 0};
  TEST(poll(&pfd, 1, 0) == -1 && errno == EOPNOTSUPP);
  pfd.revents = 0;
  TEST(poll(&pfd, 1, -1) == -1 && errno == EOPNOTSUPP);

  // Polling for writability is a different stream and works fine.
  pfd.events = POLLWRNORM;
  pfd.revents = 0;
  TEST(poll(&pfd, 1, -1) == 1);
  TEST(pfd.revents & POLLWRNORM);

  // A mixed poll which includes the read fails as a whole.
  pfd.events = POLLRDNORM | POLLWRNORM;
  pfd.revents = 0;
  TEST(poll(&pfd, 1, 0) == -1 && errno == EOPNOTSUPP);

  // Complete the first poll.
  TEST(write(client, "ping", 4) == 4);
  TEST(pthread_join(pt, NULL) == 0);

  // Everything recovers: this thread can poll and read again.
  pfd.events = POLLRDNORM;
  pfd.revents = 0;
  TEST(poll(&pfd, 1, -1) == 1);
  TEST(pfd.revents & POLLRDNORM);
  char buf[16];
  TEST(read(conn_fd, buf, sizeof(buf)) == 4);
  TEST(memcmp(buf, "ping", 4) == 0);

  TEST(close(client) == 0);
  TEST(close(conn_fd) == 0);
  return t_status;
}
