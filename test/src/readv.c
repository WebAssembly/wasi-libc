#include "test.h"
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/uio.h>
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
  TEST((fd = open(tmp, O_RDWR | O_CREAT | O_EXCL, 0600)) > 2);

  TEST(write(fd, "Hello, world!", 13) == 13);
  TEST(lseek(fd, 0, SEEK_SET) == 0);

  char buf[10];

  struct iovec iovs[3];

  // TODO: wasmtime doesn't implement `readv` correctly with shared memory as
  // it only fills the first iovec, not the first non-empty iovec.
#ifndef __wasm_atomics__
  iovs[0].iov_base = NULL;
  iovs[0].iov_len = 0;
  iovs[1].iov_base = buf;
  iovs[1].iov_len = sizeof(buf);
  TEST(readv(fd, iovs, 2) == 10);
  TEST(readv(fd, iovs, 2) == 3);
  TEST(lseek(fd, 0, SEEK_CUR) == 13);
#endif

  TEST(lseek(fd, 0, SEEK_SET) == 0);
  iovs[0].iov_base = buf;
  iovs[0].iov_len = sizeof(buf);
  iovs[1].iov_base = NULL;
  iovs[1].iov_len = 0;
  TEST(readv(fd, iovs, 2) == 10);
  TEST(readv(fd, iovs, 2) == 3);
  TEST(lseek(fd, 0, SEEK_CUR) == 13);

  TEST(lseek(fd, 0, SEEK_SET) == 0);
#ifndef __wasm_atomics__
  iovs[0].iov_base = NULL;
  iovs[0].iov_len = 0;
  iovs[1].iov_base = NULL;
  iovs[1].iov_len = 0;
  iovs[2].iov_base = buf;
  iovs[2].iov_len = sizeof(buf);
  TEST(readv(fd, iovs, 3) == 10);
  TEST(readv(fd, iovs, 3) == 3);
  TEST(lseek(fd, 0, SEEK_CUR) == 13);
#endif

  TEST(lseek(fd, 0, SEEK_SET) == 0);
  iovs[0].iov_base = buf;
  iovs[0].iov_len = 7;
  iovs[1].iov_base = buf;
  iovs[1].iov_len = 7;
  TEST(readv(fd, iovs, 2) >= 0);
  TEST(readv(fd, iovs, 2) >= 0);
  TEST(readv(fd, iovs, 2) == 0);
  TEST(lseek(fd, 0, SEEK_CUR) == 13);

  TEST(lseek(fd, 0, SEEK_SET) == 0);
  iovs[0].iov_base = NULL;
  iovs[0].iov_len = 0;
  iovs[1].iov_base = NULL;
  iovs[1].iov_len = 0;
  TEST(readv(fd, iovs, 2) == 0);
  TEST(readv(fd, iovs, 2) == 0);
  TEST(lseek(fd, 0, SEEK_CUR) == 0);

  close(fd);

  return t_status;
}
