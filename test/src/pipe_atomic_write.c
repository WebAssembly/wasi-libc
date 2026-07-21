#include "test.h"
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <string.h>
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

  // Nearly fill the pipe so that fewer than 200 bytes of space remain.
  static char big[8192];
  memset(big, 'a', sizeof(big));
  size_t space = 0;
  for (int i = 0; i < 1000; i++) {
    ssize_t n = write(fds[1], big, sizeof(big));
    if (n == -1) {
      int err = errno;
      TEST(err == EAGAIN);
      break;
    }
    space += n;
  }
  TEST(space >= 200);
  char buf[100];
  TEST(read(fds[0], buf, sizeof(buf)) == 100);

  // 200 bytes <= PIPE_BUF with only 100 bytes of space: must be EAGAIN (or a
  // complete 200-byte write if the pipe grew), never a partial write.
  char msg[200];
  TEST(sizeof(msg) <= PIPE_BUF);
  memset(msg, 'b', sizeof(msg));
  ssize_t n = write(fds[1], msg, sizeof(msg));
  int err = errno;
  TEST((n == -1 && err == EAGAIN) || n == (ssize_t)sizeof(msg));

  TEST(close(fds[0]) == 0);
  TEST(close(fds[1]) == 0);
  return t_status;
}
