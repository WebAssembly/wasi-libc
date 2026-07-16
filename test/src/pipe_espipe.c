#include "test.h"
#include <errno.h>
#include <sys/socket.h>
#include <unistd.h>

#define TEST(c)                                                                \
  do {                                                                         \
    errno = 0;                                                                 \
    if (!(c))                                                                  \
      t_error("%s failed (errno = %d)\n", #c, errno);                          \
  } while (0)

int main(void) {
  int fds[2];
  if (pipe(fds) == 0) {
    TEST(lseek(fds[0], 0, SEEK_CUR) == -1 && errno == ESPIPE);
    TEST(lseek(fds[1], 0, SEEK_END) == -1 && errno == ESPIPE);
    TEST(close(fds[0]) == 0);
    TEST(close(fds[1]) == 0);
  } else {
#ifdef __wasip3__
    // wasip3 should always take the above path
    TEST(0);
#endif
  }

#ifndef __wasip1__
  int s = socket(AF_INET, SOCK_STREAM, 0);
  TEST(s >= 0);
  TEST(lseek(s, 0, SEEK_SET) == -1 && errno == ESPIPE);
  TEST(close(s) == 0);
#endif
  return t_status;
}
