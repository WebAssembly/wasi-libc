#include "test.h"
#include <errno.h>
#include <sys/stat.h>
#include <unistd.h>

#define TEST(c)                                                                \
  do {                                                                         \
    errno = 0;                                                                 \
    if (!(c))                                                                  \
      t_error("%s failed (errno = %d)\n", #c, errno);                          \
  } while (0)

int main(void) {
#if defined(__wasip1__) || defined(__wasip2__)
  if (1)
    return t_status;
#endif

  int fds[2];
  TEST(pipe(fds) == 0);

  struct stat st;
  TEST(fstat(fds[0], &st) == 0);
  TEST(S_ISFIFO(st.st_mode));
  TEST(fstat(fds[1], &st) == 0);
  TEST(S_ISFIFO(st.st_mode));

  TEST(close(fds[0]) == 0);
  TEST(close(fds[1]) == 0);
  return t_status;
}
