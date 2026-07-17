#include "test.h"
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/select.h>
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

  int cfd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
  TEST(cfd >= 0);
  errno = 0;
  int rc = connect(cfd, (struct sockaddr *)&addr, sizeof(addr));
  if (rc != 0 && errno != EINPROGRESS)
    t_error("connect failed (rc = %d, errno = %d)\n", rc, errno);

  fd_set rfds, wfds, efds;
  FD_ZERO(&rfds);
  FD_ZERO(&wfds);
  FD_ZERO(&efds);
  FD_SET(cfd, &rfds);
  FD_SET(cfd, &wfds);
  FD_SET(cfd, &efds);
  TEST(select(cfd + 1, &rfds, &wfds, &efds, NULL) >= 0);
  TEST(FD_ISSET(cfd, &wfds));

  int err = -1;
  socklen_t err_len = sizeof(err);
  TEST(getsockopt(cfd, SOL_SOCKET, SO_ERROR, &err, &err_len) == 0);
  TEST(err == 0);

  int sfd = accept(lfd, NULL, NULL);
  TEST(sfd >= 0);
  TEST(send(cfd, "ping", 4, 0) == 4);
  char buf[4] = {0};
  TEST(recv(sfd, buf, sizeof(buf), 0) == 4);
  TEST(memcmp(buf, "ping", 4) == 0);

  TEST(close(sfd) == 0);
  TEST(close(cfd) == 0);
  TEST(close(lfd) == 0);
  return t_status;
}
