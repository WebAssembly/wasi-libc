#include "test.h"
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <poll.h>
#include <sys/socket.h>
#include <unistd.h>

#define TEST(c)                                                                \
  do {                                                                         \
    errno = 0;                                                                 \
    if (!(c))                                                                  \
      t_error("%s failed (errno = %d)\n", #c, errno);                          \
  } while (0)

int main(void) {
  int lsock = socket(AF_INET, SOCK_STREAM, 0);
  TEST(lsock >= 0);

  struct sockaddr_in addr = {.sin_family = AF_INET};
  addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  TEST(bind(lsock, (struct sockaddr *)&addr, sizeof(addr)) == 0);
  TEST(listen(lsock, 5) == 0);
  socklen_t alen = sizeof(addr);
  TEST(getsockname(lsock, (struct sockaddr *)&addr, &alen) == 0);

  int csock = socket(AF_INET, SOCK_STREAM, 0);
  TEST(csock >= 0);
  TEST(fcntl(csock, F_SETFL, O_NONBLOCK) == 0);
  connect(csock, (struct sockaddr *)&addr, sizeof(addr));
  struct pollfd pfd = {.fd = csock, .events = POLLOUT, .revents = 0};
  TEST(poll(&pfd, 1, 5000) == 1);
  TEST(pfd.revents & POLLWRNORM);

  TEST(close(lsock) == 0);

  struct sockaddr_in peer;
  socklen_t plen = sizeof(peer);
  int r = getpeername(csock, (struct sockaddr *)&peer, &plen);
  int e = errno;
  TEST(r == -1);
  TEST(e == ENOTCONN);

  TEST(close(csock) == 0);
  return t_status;
}
