#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <wasi/api.h>
#include <wasi/descriptor_table.h>

int dup(int fd) {
#ifdef __wasip1__
  (void)fd;
  errno = ENOTSUP;
  return -1;
#else
  return descriptor_table_dup(fd, DUP_OP_DUP, 0);
#endif
}

int dup2(int fd, int newfd) {
#ifdef __wasip1__
  (void)fd;
  (void)newfd;
  errno = ENOTSUP;
  return -1;
#else
  return descriptor_table_dup(fd, DUP_OP_DUP2, newfd);
#endif
}

int __dup3(int fd, int newfd, int flags) {
  if (flags & ~O_CLOEXEC) {
    errno = EINVAL;
    return -1;
  }
#ifdef __wasip1__
  (void)fd;
  (void)newfd;
  errno = ENOTSUP;
  return -1;
#else
  return descriptor_table_dup(fd, DUP_OP_DUP3, newfd);
#endif
}

weak_alias(__dup3, dup3);
