#include "test.h"
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

#define TEST(c)                                                                \
  do {                                                                         \
    errno = 0;                                                                 \
    if (!(c))                                                                  \
      t_error("%s failed (errno = %d)\n", #c, errno);                          \
  } while (0)

int main(void) {
#if defined(__wasip1__) || defined(__wasip2__)
  if (1)
    return t_status;
#endif

  int fds[2];
  TEST(pipe(fds) == 0);

  TEST(fcntl(fds[0], F_SETFL, O_NONBLOCK) == 0);
  int fl = fcntl(fds[0], F_GETFL);
  TEST(fl >= 0 && (fl & O_NONBLOCK) != 0);

  // The write end must still be blocking.
  fl = fcntl(fds[1], F_GETFL);
  TEST(fl >= 0 && (fl & O_NONBLOCK) == 0);

  // And clearing it on the write end must not clear it on the read end.
  TEST(fcntl(fds[1], F_SETFL, 0) == 0);
  fl = fcntl(fds[0], F_GETFL);
  TEST(fl >= 0 && (fl & O_NONBLOCK) != 0);

  TEST(close(fds[0]) == 0);
  TEST(close(fds[1]) == 0);
  return t_status;
}
