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
  TEST(pipe2(fds, O_NONBLOCK) == 0);

  char c;
  // A zero-length read of an empty pipe returns 0.
  TEST(read(fds[0], &c, 0) == 0);
  // A zero-length write returns 0.
  TEST(write(fds[1], &c, 0) == 0);
  // The zero-length write left no data to read.
  TEST(read(fds[0], &c, 1) == -1 && errno == EAGAIN);

  // Zero-length ops still return 0 when data is buffered.
  TEST(write(fds[1], "x", 1) == 1);
  TEST(read(fds[0], &c, 0) == 0);
  TEST(read(fds[0], &c, 1) == 1 && c == 'x');

  TEST(close(fds[0]) == 0);
  TEST(close(fds[1]) == 0);
  return t_status;
}
