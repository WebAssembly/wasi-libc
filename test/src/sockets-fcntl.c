#include "test.h"
#include <errno.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/socket.h>

#define TEST(c)                                                                \
  do {                                                                         \
    errno = 0;                                                                 \
    if (!(c))                                                                  \
      t_error("%s failed (errno = %d)\n", #c, errno);                          \
  } while (0)

#define TEST2(c)                                                               \
  do {                                                                         \
    if (!(c))                                                                  \
      t_error("%s failed (errno = %d)\n", #c, errno);                          \
  } while (0)

#define ASSERT(c)                                                              \
  do {                                                                         \
    if (!(c))                                                                  \
      t_error("%s failed\n", #c);                                              \
  } while (0)

int main() {
  int types[2] = {SOCK_STREAM, SOCK_DGRAM};

  for (int i = 0; i < 2; ++i) {
    errno = 0;
    int fd = socket(AF_INET, types[i], 0);
    TEST2(fd != -1);

    {
      errno = 0;
      int flags = fcntl(fd, F_GETFL, 0);
      TEST2(flags != -1);
      ASSERT((flags & O_NONBLOCK) == 0);
    }

    TEST(fcntl(fd, F_SETFL, O_NONBLOCK) != -1);

    {
      errno = 0;
      int flags = fcntl(fd, F_GETFL, 0);
      TEST2(flags != -1);
      ASSERT((flags & O_NONBLOCK) != 0);
    }

    int zero = 0;
    TEST(ioctl(fd, FIONBIO, &zero) != -1);

    {
      errno = 0;
      int flags = fcntl(fd, F_GETFL, 0);
      TEST2(flags != -1);
      ASSERT((flags & O_NONBLOCK) == 0);
    }
  }

  return t_status;
}
