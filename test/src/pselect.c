// Test pselect() with errorfds and poll() POLLPRI handling.
//
// WASI has no out-of-band data, so POLLPRI / exceptfds should never
// report ready. However, pselect() should not fail when errorfds is
// provided alongside other fd sets. poll() should only fail when
// POLLPRI is the exclusive set of events being requested.

#include "test.h"
#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <sys/select.h>
#include <unistd.h>

#define TEST(c, ...) ((c) ? 1 : (t_error(#c " failed: " __VA_ARGS__), 0))

int main(void) {
  char tmp[] = "testsuite-XXXXXX";
  int fd;

  TEST((fd = open(tmp, O_RDWR | O_CREAT | O_EXCL, 0600)) > 2);
  TEST(write(fd, "hello", 5) == 5);
  TEST(lseek(fd, 0, SEEK_SET) == 0);

  // ---- pselect: readfds + errorfds should succeed ----
  {
    fd_set rfds, efds;
    FD_ZERO(&rfds);
    FD_ZERO(&efds);
    FD_SET(fd, &rfds);
    FD_SET(fd, &efds);

    // Use blocking wait (no timeout) like the poll test does.
    // A zero timeout may race with event delivery on some runtimes.
    int r = pselect(fd + 1, &rfds, NULL, &efds, NULL, NULL);
    TEST(r >= 0, "pselect with readfds+errorfds returned %d, errno=%d\n", r,
         errno);
    // errorfds should be empty (POLLPRI never fires in WASI).
    TEST(!FD_ISSET(fd, &efds),
         "errorfds should be empty (no OOB data in WASI)\n");
    // readfds should have the fd set (file has data).
    TEST(FD_ISSET(fd, &rfds), "readfds should have fd set\n");
  }

  // ---- pselect: only errorfds, no readfds/writefds should fail ----
  {
    fd_set efds;
    FD_ZERO(&efds);
    FD_SET(fd, &efds);

    struct timespec tv = {0, 0};
    errno = 0;
    int r = pselect(fd + 1, NULL, NULL, &efds, &tv, NULL);
    TEST(r == -1 && errno == ENOSYS,
         "pselect with only errorfds should fail with ENOSYS, got r=%d "
         "errno=%d\n",
         r, errno);
  }

  // ---- poll: POLLPRI | POLLRDNORM should succeed ----
  {
    struct pollfd pfd = {
        .fd = fd, .events = POLLPRI | POLLRDNORM, .revents = 0};
    int r = poll(&pfd, 1, -1);
    TEST(r >= 0, "poll with POLLPRI|POLLRDNORM returned %d, errno=%d\n", r,
         errno);
    // POLLPRI should not be set in revents.
    TEST(!(pfd.revents & POLLPRI), "POLLPRI should not be reported in WASI\n");
  }

  // ---- poll: exclusively POLLPRI should fail ----
  {
    struct pollfd pfd = {.fd = fd, .events = POLLPRI, .revents = 0};
    errno = 0;
    int r = poll(&pfd, 1, 0);
    TEST(r == -1 && errno == ENOSYS,
         "poll with only POLLPRI should fail with ENOSYS, got r=%d errno=%d\n",
         r, errno);
  }

  close(fd);
  TEST(unlink(tmp) != -1);

  return t_status;
}
