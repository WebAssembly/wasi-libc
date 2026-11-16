#include "test.h"
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define TEST(c)                                                                \
  do {                                                                         \
    errno = 0;                                                                 \
    if (!(c))                                                                  \
      t_error("%s failed (errno = %d)\n", #c, errno);                          \
  } while (0)

// Run with an initial cwd of `/nowhere`, which no preopen covers, and this
// test's source directory preopened at `/src`.
int main(void) {
  char cwd[16];
  TEST(getcwd(cwd, sizeof(cwd)) != NULL);
  TEST(strcmp(cwd, "/nowhere") == 0);

  // Relative paths don't fall back to some other directory.
  TEST(open("initial_cwd_uncovered.c", O_RDONLY) == -1 && errno == ENOENT);

  TEST(chdir("/src") == 0);
  int fd = open("initial_cwd_uncovered.c", O_RDONLY);
  TEST(fd >= 0);
  TEST(close(fd) == 0);

  return t_status;
}
