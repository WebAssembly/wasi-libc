#include "test.h"
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#define TEST(c)                                                                \
  do {                                                                         \
    errno = 0;                                                                 \
    if (!(c))                                                                  \
      t_error("%s failed (errno = %d)\n", #c, errno);                          \
  } while (0)

// POSIX read() on a directory fd must fail with EISDIR, not EBADF.
// wasip2 read-via-stream currently surfaces bad-descriptor, which
// translate_error maps to EBADF.
int main(void) {
  char buf[16];

  TEST(mkdir("read-dir-eisdir", 0755) == 0);
  int dirfd = open("read-dir-eisdir", O_RDONLY | O_DIRECTORY);
  TEST(dirfd > 2);

  TEST(read(dirfd, buf, sizeof buf) == -1 && errno == EISDIR);

  TEST(close(dirfd) == 0);

  // Genuine bad fds stay EBADF; regular files still read.
  TEST(read(-1, buf, 1) == -1 && errno == EBADF);

  int fd;
  TEST((fd = open("read-dir-eisdir/f", O_RDWR | O_CREAT | O_EXCL, 0600)) > 2);
  TEST(write(fd, "x", 1) == 1);
  TEST(lseek(fd, 0, SEEK_SET) == 0);
  TEST(read(fd, buf, 1) == 1);
  TEST(buf[0] == 'x');
  TEST(close(fd) == 0);
  TEST(unlink("read-dir-eisdir/f") == 0);
  TEST(rmdir("read-dir-eisdir") == 0);

  return t_status;
}
