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

static void listen_and_connect(int backlog) {
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

  errno = 0;
  if (listen(lfd, backlog) != 0) {
    t_error("listen(%d) failed (errno = %d)\n", backlog, errno);
    TEST(close(lfd) == 0);
    return;
  }

  int cfd = socket(AF_INET, SOCK_STREAM, 0);
  TEST(cfd >= 0);
  TEST(connect(cfd, (struct sockaddr *)&addr, sizeof(addr)) == 0);
  int sfd = accept(lfd, NULL, NULL);
  TEST(sfd >= 0);

  TEST(close(sfd) == 0);
  TEST(close(cfd) == 0);
  TEST(close(lfd) == 0);
}

int main(void) {
  listen_and_connect(0);
  listen_and_connect(-1);
  return t_status;
}
