#include "test.h"
#include <errno.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#define TEST(c)                                                                \
  do {                                                                         \
    errno = 0;                                                                 \
    if (!(c))                                                                  \
      t_error("%s failed (errno = %d)\n", #c, errno);                          \
  } while (0)

int main(void) {
  int fd = socket(AF_INET6, SOCK_STREAM, 0);
  TEST(fd >= 0);

  int val = -1;
  socklen_t len = sizeof(val);
  TEST(getsockopt(fd, IPPROTO_IPV6, IPV6_V6ONLY, &val, &len) == 0);
  TEST(val == 1);

  val = 1;
  TEST(setsockopt(fd, IPPROTO_IPV6, IPV6_V6ONLY, &val, sizeof(val)) == 0);

  val = 0;
  int r = setsockopt(fd, IPPROTO_IPV6, IPV6_V6ONLY, &val, sizeof(val));
  int e = errno;
  TEST(r == -1);
  TEST(e == EOPNOTSUPP);

  val = -1;
  TEST(getsockopt(fd, IPPROTO_IPV6, IPV6_V6ONLY, &val, &len) == 0);
  TEST(val == 1);

  close(fd);

  int fd4 = socket(AF_INET, SOCK_STREAM, 0);
  TEST(fd4 >= 0);
  val = 1;
  r = setsockopt(fd4, IPPROTO_IPV6, IPV6_V6ONLY, &val, sizeof(val));
  e = errno;
  TEST(r == -1);
  TEST(e == EAFNOSUPPORT);
  close(fd4);

  return t_status;
}
