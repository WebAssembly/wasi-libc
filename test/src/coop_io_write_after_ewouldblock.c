#include "test.h"
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
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

static int drain_fd;

static void *drainer(void *arg) {
  long long *total_read = (long long *)arg;
  static char buf[65536];
  ssize_t n;
  while ((n = read(drain_fd, buf, sizeof(buf))) > 0)
    *total_read += n;
  TEST(n == 0);
  return NULL;
}

int main(void) {
  int wfd;
  tcp_loopback_pair(&drain_fd, &wfd);

  // Put as much data into this socket as we reasonably can.
  TEST(fcntl(wfd, F_SETFL, O_NONBLOCK) == 0);
  static char chunk[65536];
  memset(chunk, 0x5a, sizeof(chunk));

  long long total_written = 0;
  int ewouldblocks = 0;
  while (ewouldblocks < 5 && total_written < (100 << 20)) {
    ssize_t n = write(wfd, chunk, sizeof(chunk));
    int write_errno = errno;
    if (n < 0) {
      TEST(n == -1 && write_errno == EWOULDBLOCK);
      ewouldblocks++;
    } else {
      TEST(n > 0);
      total_written += n;
    }
  }

  // Start draining in another thread so the blocking write can complete.
  pthread_t dt;
  long long total_read = 0;
  TEST(pthread_create(&dt, NULL, drainer, &total_read) == 0);

  // Now do a blocking write.
  TEST(fcntl(wfd, F_SETFL, 0) == 0);
  ssize_t n = write(wfd, chunk, sizeof(chunk));
  TEST(n > 0);
  total_written += n;

  TEST(shutdown(wfd, SHUT_WR) == 0);
  TEST(pthread_join(dt, NULL) == 0);

  if (total_read != total_written)
    t_error("read %lld bytes, expected %lld\n", total_read, total_written);

  TEST(close(wfd) == 0);
  TEST(close(drain_fd) == 0);
  return t_status;
}
