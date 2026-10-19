#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <wasi/libc.h>

#if defined(__webp2__)
#include <wasi/report-error.h>
#endif

int truncate(const char *path, off_t length) {
#if !defined(__webp2__)
  int fd = __wasilibc_open_nomode(path, O_WRONLY | O_CLOEXEC | O_NOCTTY);
  if (fd < 0)
    return -1;

  int result = ftruncate(fd, length);
  if (result != 0) {
    int save_errno = errno;
    (void)close(fd);
    errno = save_errno;
    return -1;
  }

  return close(fd);
#else
  WASILIBC_LOG_UNSUPPORTED("truncate");
  errno = ENOENT;
  return -1;
#endif // !webp2
}
