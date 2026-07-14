#include "test.h"
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <poll.h>
#include <pthread.h>
#include <sched.h>
#include <stdbool.h>
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

#define READERS 3
#define ITERS 2000

static int conn_fd;

static void *reader(void *arg) {
  (void)arg;
  char buf[1];
  while (1) {
    ssize_t n = read(conn_fd, buf, sizeof(buf));
    if (n <= 0)
      break;
  }
  return NULL;
}

int main(void) {
  int client;
  tcp_loopback_pair(&conn_fd, &client);

  pthread_t rt[READERS];
  for (int i = 0; i < READERS; i++)
    TEST(pthread_create(&rt[i], NULL, reader, NULL) == 0);

  // let threads get blocked
  for (int i = 0; i < 10; i++)
    TEST(sched_yield() == 0);

  bool shutdown_ok = false;
  for (int i = 0; i < ITERS && !shutdown_ok; i++) {
    TEST(write(client, "x", 1) == 1);
    errno = 0;
    int rc = shutdown(conn_fd, SHUT_RD);
    int shutdown_errno = errno;
    if (rc == 0) {
      // Legitimate success: no reader was blocked at that instant. Readers
      // will now observe EOF.
      shutdown_ok = true;
    } else if (shutdown_errno != EOPNOTSUPP) {
      t_error("unexpected shutdown failure (errno = %d)\n", shutdown_errno);
      break;
    }
    TEST(sched_yield() == 0);
  }

  TEST(close(client) == 0);
  for (int i = 0; i < READERS; i++)
    TEST(pthread_join(rt[i], NULL) == 0);
  TEST(close(conn_fd) == 0);
  return t_status;
}
