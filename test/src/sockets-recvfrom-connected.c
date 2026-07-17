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

  TEST(send(cfd, "ping", 4, 0) == 4);

  struct sockaddr_in src;
  socklen_t src_len = sizeof(src);
  char buf[8] = {0};
  TEST(recvfrom(sfd, buf, sizeof(buf), 0, (struct sockaddr *)&src, &src_len) ==
       4);
  TEST(memcmp(buf, "ping", 4) == 0);
  TEST(src_len == 0);

  TEST(close(sfd) == 0);
  TEST(close(cfd) == 0);
  TEST(close(lfd) == 0);
  return t_status;
}
