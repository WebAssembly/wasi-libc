#include "test.h"
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

int main(void) {
  const char *path = "stream-error.tmp";
  char byte;
  int fd = open(path, O_CREAT | O_TRUNC | O_WRONLY, 0600);
  if (fd == -1) {
    t_error("open failed: errno=%d\n", errno);
    return t_status;
  }

  // Hosts report access-mode violations as bad descriptor or access denied.
  errno = 0;
  ssize_t result = read(fd, &byte, 1);
  int read_errno = errno;
  if (result != -1 || (read_errno != EBADF && read_errno != EACCES))
    t_error(
        "read: expected -1 and EBADF (%d) or EACCES (%d), got %zd/errno=%d\n",
        EBADF, EACCES, result, read_errno);

  if (close(fd) == -1)
    t_error("close failed: errno=%d\n", errno);

  fd = open(path, O_RDONLY);
  if (fd == -1) {
    t_error("reopen failed: errno=%d\n", errno);
    if (unlink(path) == -1)
      t_error("unlink failed: errno=%d\n", errno);
    return t_status;
  }

  errno = 0;
  result = write(fd, "x", 1);
  int write_errno = errno;
  if (result != -1 || (write_errno != EBADF && write_errno != EACCES))
    t_error(
        "write: expected -1 and EBADF (%d) or EACCES (%d), got %zd/errno=%d\n",
        EBADF, EACCES, result, write_errno);

  if (close(fd) == -1)
    t_error("close failed: errno=%d\n", errno);
  if (unlink(path) == -1)
    t_error("unlink failed: errno=%d\n", errno);

  return t_status;
}
