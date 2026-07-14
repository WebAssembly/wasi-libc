#include "test.h"
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <poll.h>
#include <pthread.h>
#include <sched.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
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

int main(void) {
  int server, client;
  tcp_loopback_pair(&server, &client);

  struct timeval tv = {.tv_sec = 0, .tv_usec = 100 * 1000};
  TEST(setsockopt(server, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) == 0);

  TEST(write(client, "x", 1) == 1);
  struct pollfd pfd = {.fd = server, .events = POLLRDNORM, .revents = 0};
  TEST(poll(&pfd, 1, -1) == 1);
  char buf[4];
  TEST(read(server, buf, sizeof(buf)) == 1);

  // A blocking read with no data times out.
  errno = 0;
  TEST((read(server, buf, sizeof(buf)) == -1) &&
       (errno == ETIMEDOUT || errno == EWOULDBLOCK));

  // Poll should say nothing.
  pfd.revents = 0;
  TEST(poll(&pfd, 1, 0) == 0);

  // Real readiness still works: new data makes poll fire and read succeed.
  TEST(write(client, "z", 1) == 1);
  pfd.revents = 0;
  TEST(poll(&pfd, 1, -1) == 1);
  TEST(pfd.revents & POLLRDNORM);
  TEST(read(server, buf, sizeof(buf)) == 1);
  TEST(buf[0] == 'z');

  TEST(close(client) == 0);
  TEST(close(server) == 0);
  return t_status;
}
