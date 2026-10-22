#include "test.h"
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <unistd.h>

#define TEST(c)                                                                \
  do {                                                                         \
    errno = 0;                                                                 \
    if (!(c))                                                                  \
      t_error("%s failed (errno = %d)\n", #c, errno);                          \
  } while (0)

int main(void) {
  char tmp[] = "testsuite-XXXXXX";
  int fd;
  int enabled = 1;
  int disabled = 0;

  TEST((fd = open(tmp, O_RDWR | O_CREAT, 0600)) > 2);

  // O_NONBLOCK
  TEST(fcntl(fd, F_GETFL) == O_RDWR);
  TEST(ioctl(fd, FIONBIO, &enabled) == 0);
  TEST(fcntl(fd, F_GETFL) == (O_RDWR | O_NONBLOCK));
  TEST(ioctl(fd, FIONBIO, &disabled) == 0);
  TEST(fcntl(fd, F_GETFL) == O_RDWR);
  TEST(fcntl(fd, F_SETFL, O_NONBLOCK) == 0);
  TEST(fcntl(fd, F_GETFL) == (O_RDWR | O_NONBLOCK));
  TEST(fcntl(fd, F_SETFL, 0) == 0);
  TEST(fcntl(fd, F_GETFL) == O_RDWR);

  // O_APPEND
  TEST(fcntl(fd, F_SETFL, O_APPEND) == 0);
  TEST(fcntl(fd, F_GETFL) == (O_RDWR | O_APPEND));
  TEST(fcntl(fd, F_SETFL, O_APPEND | O_NONBLOCK) == 0);
  TEST(fcntl(fd, F_GETFL) == (O_RDWR | O_APPEND | O_NONBLOCK));
  TEST(fcntl(fd, F_SETFL, 0) == 0);
  TEST(fcntl(fd, F_GETFL) == O_RDWR);

  TEST(close(fd) == 0);

  // stdio - not that wasmtime's implementation for wasip1-threads is
  // broken so some tests are skipped there.
  TEST(fcntl(0, F_GETFL) == O_RDONLY);
#ifndef __wasm_atomics__
  TEST(fcntl(1, F_GETFL) == O_WRONLY);
  TEST(fcntl(2, F_GETFL) == O_WRONLY);
#endif
#ifdef __wasip1__
  // wasip1 asks the host (fd_fdstat_set_flags), which refuses it for stdio.
  TEST(fcntl(0, F_SETFL, O_APPEND) == -1);
  TEST(fcntl(1, F_SETFL, O_APPEND) == -1);
  TEST(fcntl(2, F_SETFL, O_APPEND) == -1);
  TEST(fcntl(0, F_SETFL, O_NONBLOCK) == -1);
  TEST(fcntl(1, F_SETFL, O_NONBLOCK) == -1);
  TEST(fcntl(2, F_SETFL, O_NONBLOCK) == -1);
#else
  // wasip2 and wasip3: stdio keeps O_NONBLOCK, as a pipe or a socket does,
  // and ignores the other status flags (O_APPEND means nothing for a stream).
  for (int i = 0; i <= 2; i++) {
    int mode = i == 0 ? O_RDONLY : O_WRONLY;
    TEST(fcntl(i, F_SETFL, O_APPEND) == 0);
    TEST(fcntl(i, F_GETFL) == mode);
    TEST(fcntl(i, F_SETFL, O_NONBLOCK) == 0);
    TEST(fcntl(i, F_GETFL) == (mode | O_NONBLOCK));
    TEST(fcntl(i, F_SETFL, 0) == 0);
    TEST(fcntl(i, F_GETFL) == mode);
  }
#endif

  return t_status;
}
