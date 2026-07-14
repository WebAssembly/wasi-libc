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
static _Atomic int got_byte;
static _Atomic int got_eof;

static void *reader(void *arg) {
  (void)arg;
  char buf[1];
  errno = 0;
  ssize_t n = read(conn_fd, buf, sizeof(buf));
  if (n == 1)
    got_byte++;
  else if (n == 0)
    got_eof++;
  else
    t_error("unexpected read result %d (errno = %d)\n", (int)n, errno);
  return NULL;
}

int main(void) {
  int client;
  tcp_loopback_pair(&conn_fd, &client);

  pthread_t r1, r2;
  TEST(pthread_create(&r1, NULL, reader, NULL) == 0);
  for (int i = 0; i < 20; i++)
    sched_yield();
  TEST(pthread_create(&r2, NULL, reader, NULL) == 0);
  for (int i = 0; i < 20; i++)
    sched_yield();

  TEST(write(client, "x", 1) == 1);
  int rc;
  int fed = 1;
  do {
    errno = 0;
    rc = shutdown(conn_fd, SHUT_RD);
    if (rc == -1) {
      if (errno != EOPNOTSUPP)
        t_error("unexpected shutdown failure (errno = %d)\n", errno);
      if (got_byte < fed) {
        sched_yield();
      } else {
        TEST(write(client, "x", 1) == 1);
        fed++;
      }
    }
  } while (rc == -1);

  TEST(pthread_join(r1, NULL) == 0);
  TEST(pthread_join(r2, NULL) == 0);

  TEST(got_byte + got_eof == 2);
  TEST(got_byte >= 1);

  TEST(close(client) == 0);
  TEST(close(conn_fd) == 0);
  return t_status;
}
