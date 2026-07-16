#include "test.h"
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#define TEST(c)                                                                \
  do {                                                                         \
    errno = 0;                                                                 \
    if (!(c))                                                                  \
      t_error("%s failed (errno = %d)\n", #c, errno);                          \
  } while (0)

int main(void) {
#ifdef __wasip1__
  if (1)
    return t_status;
#endif

  int a = open("dup3-a.txt", O_RDWR | O_CREAT | O_EXCL, 0600);
  TEST(a > 2);
  int b = open("dup3-b.txt", O_RDWR | O_CREAT | O_EXCL, 0600);
  TEST(b > 2);

  TEST(dup3(-1, b, 0) == -1 && errno == EBADF);
  TEST(dup3(1234, b, 0) == -1 && errno == EBADF);
  TEST(dup3(a, -1, 0) == -1 && errno == EBADF);
  TEST(dup3(a, a, 0) == -1 && errno == EINVAL);
  TEST(dup3(a, b, O_RDWR) == -1 && errno == EINVAL);

  TEST(write(b, "b", 1) == 1);

  TEST(dup3(a, b, 0) == b);
  TEST(write(b, "hello", 5) == 5);
  TEST(lseek(a, 0, SEEK_CUR) == 5);

  int big = a + 20;
  TEST(dup3(a, big, O_CLOEXEC) == big);
  TEST(fcntl(big, F_GETFD) == FD_CLOEXEC);
  TEST(write(big, " world", 6) == 6);
  TEST(lseek(a, 0, SEEK_CUR) == 11);

  TEST(lseek(a, 0, SEEK_SET) == 0);
  char buf[16] = {0};
  TEST(read(a, buf, sizeof(buf)) == 11);
  TEST(memcmp(buf, "hello world", 11) == 0);

  TEST(close(a) == 0);
  TEST(close(b) == 0);
  TEST(lseek(big, 0, SEEK_SET) == 0);
  TEST(read(big, buf, sizeof(buf)) == 11);
  TEST(memcmp(buf, "hello world", 11) == 0);
  TEST(close(big) == 0);

  int fds[32];
  for (int i = 0; i < 32; i++) {
    fds[i] = open("dup3-b.txt", O_RDONLY);
    TEST(fds[i] > 2);
    for (int j = 0; j < i; j++)
      TEST(fds[i] != fds[j]);
    TEST(read(fds[i], buf, 1) == 1);
    TEST(buf[0] == 'b');
  }
  for (int i = 0; i < 32; i++)
    TEST(close(fds[i]) == 0);

  TEST(unlink("dup3-a.txt") == 0);
  TEST(unlink("dup3-b.txt") == 0);
  return t_status;
}
