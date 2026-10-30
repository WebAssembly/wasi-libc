//! stdin, stdout and stderr take O_NONBLOCK, as files do.
//!
//! `fcntl(fd, F_SETFL, flags | O_NONBLOCK)` on a stdio descriptor succeeds,
//! F_GETFL reports it, and ioctl(FIONBIO) sets it too.

#include "test.h"
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define TEST(c)                                                                \
  do {                                                                         \
    errno = 0;                                                                 \
    if (!(c))                                                                  \
      t_error("%s failed (errno = %d)\n", #c, errno);                          \
  } while (0)

int main(void) {
#ifdef __wasip1__
  TEST(fcntl(0, F_SETFL, O_APPEND) == -1);
  TEST(fcntl(1, F_SETFL, O_APPEND) == -1);
  TEST(fcntl(2, F_SETFL, O_APPEND) == -1);
  TEST(fcntl(0, F_SETFL, O_NONBLOCK) == -1);
  TEST(fcntl(1, F_SETFL, O_NONBLOCK) == -1);
  TEST(fcntl(2, F_SETFL, O_NONBLOCK) == -1);
#else
  for (int fd = 0; fd <= 2; fd++) {
    int fl = fcntl(fd, F_GETFL);
    TEST(fl >= 0);
    TEST((fl & O_NONBLOCK) == 0);
    // Other status flags (O_APPEND) mean nothing for a stream and are ignored.
    TEST(fcntl(fd, F_SETFL, O_APPEND) == 0);
    TEST(fcntl(fd, F_GETFL) == fl);
    TEST(fcntl(fd, F_SETFL, fl | O_NONBLOCK) == 0);
    TEST(fcntl(fd, F_GETFL) == (fl | O_NONBLOCK));
    TEST(fcntl(fd, F_SETFL, fl) == 0);
    TEST(fcntl(fd, F_GETFL) == fl);
  }

  int on = 1;
  TEST(ioctl(0, FIONBIO, &on) == 0);
  TEST((fcntl(0, F_GETFL) & O_NONBLOCK) != 0);
  on = 0;
  TEST(ioctl(0, FIONBIO, &on) == 0);
  TEST((fcntl(0, F_GETFL) & O_NONBLOCK) == 0);

  // A non-blocking stdout still takes a write.
  TEST(fcntl(1, F_SETFL, O_NONBLOCK) == 0);
  errno = 0;
  ssize_t n = write(1, "ok\n", 3);
  if (n != 3 && !(n < 0 && errno == EAGAIN))
    t_error("a non-blocking write to stdout returned %d (errno %d, %s)\n",
            (int)n, errno, strerror(errno));
  TEST(fcntl(1, F_SETFL, 0) == 0);
#endif

  return t_status;
}
