#include "test.h"
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
  TEST(connect(csock, (struct sockaddr *)&addr, sizeof(addr)) == 0);
  int ssock = accept(lsock, NULL, NULL);
  TEST(ssock >= 0);

  TEST(close(ssock) == 0);
  TEST(close(lsock) == 0);

  char buf[4096];
  memset(buf, 'x', sizeof(buf));
  int saw_error = 0;
  for (int i = 0; i < 64; i++) {
    errno = 0;
    ssize_t n = send(csock, buf, sizeof(buf), 0);
    if (n < 0) {
      int e = errno;
      TEST(e == EPIPE || e == ECONNRESET);
      saw_error = 1;
      break;
    }
  }
  TEST(saw_error);

  TEST(close(csock) == 0);
  return t_status;
}
