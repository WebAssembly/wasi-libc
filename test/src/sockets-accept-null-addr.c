#include "test.h"
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define TEST(c)                                                                \
  do {                                                                         \
    errno = 0;                                                                 \
    if (!(c))                                                                  \
      t_error("%s failed (errno = %d)\n", #c, errno);                          \
  } while (0)

// POSIX allows accept/accept4 peer address args to both be NULL when the
// caller does not need the peer address. wasip1 previously null-dereferenced
// on that shape after a successful sock_accept.
static int listen_loopback(void) {
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
  return lfd;
}

static int connect_nonblock(const struct sockaddr_in *addr) {
  int cfd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
  TEST(cfd >= 0);
  errno = 0;
  int rc = connect(cfd, (const struct sockaddr *)addr, sizeof(*addr));
  if (rc != 0 && errno != EINPROGRESS)
    t_error("connect failed (rc = %d, errno = %d)\n", rc, errno);
  return cfd;
}

static void check_ping(int cfd, int sfd) {
  TEST(send(cfd, "ping", 4, 0) == 4);
  char buf[4] = {0};
  TEST(recv(sfd, buf, sizeof(buf), 0) == 4);
  TEST(memcmp(buf, "ping", 4) == 0);
}

int main(void) {
  // accept(fd, NULL, NULL)
  {
    int lfd = listen_loopback();
    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);
    TEST(getsockname(lfd, (struct sockaddr *)&addr, &addr_len) == 0);
    int cfd = connect_nonblock(&addr);
    int sfd = accept(lfd, NULL, NULL);
    TEST(sfd >= 0);
    check_ping(cfd, sfd);
    TEST(close(sfd) == 0);
    TEST(close(cfd) == 0);
    TEST(close(lfd) == 0);
  }

  // accept4(fd, NULL, NULL, 0)
  {
    int lfd = listen_loopback();
    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);
    TEST(getsockname(lfd, (struct sockaddr *)&addr, &addr_len) == 0);
    int cfd = connect_nonblock(&addr);
    int sfd = accept4(lfd, NULL, NULL, 0);
    TEST(sfd >= 0);
    check_ping(cfd, sfd);
    TEST(close(sfd) == 0);
    TEST(close(cfd) == 0);
    TEST(close(lfd) == 0);
  }

  // Non-null path still fills a sockaddr without crashing.
  {
    int lfd = listen_loopback();
    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);
    TEST(getsockname(lfd, (struct sockaddr *)&addr, &addr_len) == 0);
    int cfd = connect_nonblock(&addr);
    struct sockaddr_storage peer;
    socklen_t peer_len = sizeof(peer);
    int sfd = accept(lfd, (struct sockaddr *)&peer, &peer_len);
    TEST(sfd >= 0);
    TEST(peer_len > 0);
    check_ping(cfd, sfd);
    TEST(close(sfd) == 0);
    TEST(close(cfd) == 0);
    TEST(close(lfd) == 0);
  }

  return t_status;
}
