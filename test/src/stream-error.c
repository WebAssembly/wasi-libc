#include "test.h"
#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <unistd.h>

int main(void) {
  const char *path = "stream-error.tmp";
  int fd = open(path, O_CREAT | O_TRUNC | O_WRONLY, 0600);
  if (fd == -1) {
    t_error("open failed: errno=%d\n", errno);
    return t_status;
  }

  // Force a filesystem error to verify that its specific errno is preserved.
  // Hosts can reject this range with EFBIG or EINVAL.
  off_t offset = lseek(fd, INT64_MAX, SEEK_SET);
  if (offset != INT64_MAX) {
    t_error("seek to INT64_MAX failed: errno=%d\n", errno);
  } else {
    errno = 0;
    ssize_t result = write(fd, "x", 1);
    int write_errno = errno;

    if (result != -1) {
      t_error("write should fail, returned %zd\n", result);
    }

    if (write_errno != EFBIG && write_errno != EINVAL) {
      t_error("expected EFBIG (%d) or EINVAL (%d), got errno=%d\n", EFBIG,
              EINVAL, write_errno);
    }
  }

  if (close(fd) == -1)
    t_error("close failed: errno=%d\n", errno);
  if (unlink(path) == -1)
    t_error("unlink failed: errno=%d\n", errno);

  return t_status;
}
