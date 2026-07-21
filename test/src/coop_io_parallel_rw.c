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

static void *reader(void *arg) {
  (void)arg;
  char buf[16];
  ssize_t n = read(conn_fd, buf, sizeof(buf));
  TEST(n == 4);
  TEST(memcmp(buf, "ping", 4) == 0);
  return NULL;
}

int main(void) {
  int client;
  tcp_loopback_pair(&conn_fd, &client);

  pthread_t rt;
  TEST(pthread_create(&rt, NULL, reader, NULL) == 0);

  // let the thread get blocked in `read`
  for (int i = 0; i < 10; i++)
    TEST(sched_yield() == 0);

  // While the reader is blocked, a writer on the same fd must still work.
  TEST(write(conn_fd, "pong", 4) == 4);

  // The write must arrive on the client side.
  char buf[16];
  TEST(read(client, buf, sizeof(buf)) == 4);
  TEST(memcmp(buf, "pong", 4) == 0);

  // Now wake the blocked reader.
  TEST(write(client, "ping", 4) == 4);
  TEST(pthread_join(rt, NULL) == 0);

  TEST(close(client) == 0);
  TEST(close(conn_fd) == 0);
  return t_status;
}
