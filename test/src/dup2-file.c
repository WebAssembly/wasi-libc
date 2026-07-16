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

  int a = open("dup2-a.txt", O_RDWR | O_CREAT | O_EXCL, 0600);
  TEST(a > 2);
  int b = open("dup2-b.txt", O_RDWR | O_CREAT | O_EXCL, 0600);
  TEST(b > 2);

  TEST(dup2(-1, b) == -1 && errno == EBADF);
  TEST(dup2(1234, b) == -1 && errno == EBADF);
  TEST(write(b, "b", 1) == 1);
  TEST(dup2(a, -1) == -1 && errno == EBADF);

  TEST(dup2(a, a) == a);
  TEST(dup2(1234, 1234) == -1 && errno == EBADF);

  TEST(dup2(a, b) == b);
  TEST(write(b, "hello", 5) == 5);
  TEST(lseek(a, 0, SEEK_CUR) == 5);

  int big = 40;
  TEST(dup2(a, big) == big);
  TEST(write(big, " world", 6) == 6);
  TEST(lseek(a, 0, SEEK_CUR) == 11);
  TEST(close(big) == 0);

  TEST(lseek(b, 0, SEEK_SET) == 0);
  char buf[16] = {0};
  TEST(read(b, buf, sizeof(buf)) == 11);
  TEST(memcmp(buf, "hello world", 11) == 0);

  int check = open("dup2-b.txt", O_RDONLY);
  TEST(check >= 0);
  TEST(read(check, buf, sizeof(buf)) == 1);
  TEST(buf[0] == 'b');
  TEST(close(check) == 0);

  TEST(close(a) == 0);
  TEST(close(b) == 0);
  TEST(unlink("dup2-a.txt") == 0);
  TEST(unlink("dup2-b.txt") == 0);
  return t_status;
}
