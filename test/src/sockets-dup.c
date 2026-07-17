#include "test.h"
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
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

int main(void) {
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

  int cfd2 = dup(cfd);
  TEST(cfd2 >= 0);

  TEST(send(cfd, "ping", 4, 0) == 4);
  TEST(send(cfd2, "pong", 4, 0) == 4);
  char buf[16] = {0};
  size_t got = 0;
  while (got < 8) {
    ssize_t n = recv(sfd, buf + got, sizeof(buf) - got, 0);
    TEST(n > 0);
    got += n;
  }
  TEST(memcmp(buf, "pingpong", 8) == 0);

  struct sockaddr_in peer;
  socklen_t peer_len = sizeof(peer);
  TEST(getpeername(cfd2, (struct sockaddr *)&peer, &peer_len) == 0);
  TEST(peer.sin_port == addr.sin_port);

  TEST(fcntl(cfd, F_SETFL, O_NONBLOCK) == 0);
  TEST((fcntl(cfd2, F_GETFL) & O_NONBLOCK) != 0);
  errno = 0;
  TEST(recv(cfd2, buf, sizeof(buf), 0) == -1 && errno == EWOULDBLOCK);
  TEST(fcntl(cfd, F_SETFL, 0) == 0);

  TEST(close(cfd) == 0);
  TEST(send(cfd2, "!", 1, 0) == 1);
  TEST(recv(sfd, buf, sizeof(buf), 0) == 1);
  TEST(buf[0] == '!');

  TEST(close(cfd2) == 0);
  TEST(recv(sfd, buf, sizeof(buf), 0) == 0);

  TEST(close(sfd) == 0);
  TEST(close(lfd) == 0);
  return t_status;
}
