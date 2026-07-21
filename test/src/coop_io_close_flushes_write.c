#include "test.h"
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <poll.h>
#include <pthread.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define TEST(c)                                                                \
  do {                                                                         \
    errno = 0;                                                                 \
    if (!(c))                                                                  \
      t_error("%s failed (errno = %d)\n", #c, errno);                          \
  } while (0)

#define CHUNK (10 * 1024)
#define TOTAL (4 << 20)

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
  static char buf[4096];
  ssize_t n;

  // Read until EOF, but prioritize running the writer.
  while ((n = read(drain_fd, buf, sizeof(buf))) > 0) {
    *total_read += n;
    for (int i = 0; i < 5; i++)
      TEST(sched_yield() == 0);
  }
  TEST(n == 0);
  return NULL;
}

int main(void) {
  int wfd;
  tcp_loopback_pair(&drain_fd, &wfd);

  TEST(fcntl(wfd, F_SETFL, O_NONBLOCK) == 0);
  static char chunk[CHUNK];
  memset(chunk, 0x5a, sizeof(chunk));

  pthread_t dt;
  long long total_read = 0;
  TEST(pthread_create(&dt, NULL, drainer, &total_read) == 0);

  // Write a bunch, keeping track of what was written. If the socket is not
  // ready then break out. If the socket is ready then mix in a poll which will,
  // currently, trigger zero-length writes which can perturb things.
  //
  // Notably Wasmtime, for example, uses Tokio which will always say that writes
  // are ready until something actually hits EWOULDBLOCK. For wasi-libc,
  // however, a zero-length write succeeded is intended to always imply that
  // the next write will succeed, but this isn't true. Tokio could accept a
  // zero-length write and only find out on the next nonzero write that the
  // socket needs to block. This fools wasi-libc into thinking that the TCP
  // stream isn't well-behaved so it should start buffering internally. Once
  // doing so that's what this test is about.
  long long total_written = 0;
  struct pollfd pfd = {.fd = wfd, .events = POLLWRNORM, .revents = 0};
  while (1) {
    ssize_t n = write(wfd, chunk, sizeof(chunk));
    int write_errno = errno;
    if (n < 0) {
      TEST(write_errno == EWOULDBLOCK);
      break;
    } else {
      total_written += n;
      if (poll(&pfd, 1, 0) == 0)
        break;
    }
  }

  // Finish up reading.
  TEST(close(wfd) == 0);
  TEST(pthread_join(dt, NULL) == 0);

  // Effectively the `close(wfd)` should have blocked to send out the buffered
  // write that it did. If it didn't then this fails.
  if (total_read != total_written)
    t_error("read %lld bytes, expected %lld\n", total_read, total_written);

  TEST(close(drain_fd) == 0);
  return t_status;
}
