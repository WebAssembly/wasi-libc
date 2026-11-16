#include "test.h"
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <wasi/libc.h>

#define TEST(c)                                                                \
  do {                                                                         \
    errno = 0;                                                                 \
    if (!(c))                                                                  \
      t_error("%s failed (errno = %d)\n", #c, errno);                          \
  } while (0)

static void write_file(const char *path, const char *contents) {
  int fd = open(path, O_CREAT | O_WRONLY | O_TRUNC, 0644);
  TEST(fd >= 0);
  TEST(write(fd, contents, strlen(contents)) == (ssize_t)strlen(contents));
  TEST(close(fd) == 0);
}

// Checks that `getcwd` is `expected` and that the relative path `f.txt`
// resolves there.
static void check_cwd(const char *expected) {
  char cwd[16];
  TEST(getcwd(cwd, sizeof(cwd)) != NULL);
  TEST(strcmp(cwd, expected) == 0);

  char buf[16] = {0};
  int fd = open("f.txt", O_RDONLY);
  TEST(fd >= 0);
  TEST(read(fd, buf, sizeof(buf) - 1) >= 0);
  TEST(close(fd) == 0);
  TEST(strcmp(buf, expected) == 0);
}

// Run with an initial cwd of `/sub`.
int main(void) {
  TEST(mkdir("/sub", 0755) == 0 || errno == EEXIST);
  TEST(mkdir("/sub/x", 0755) == 0 || errno == EEXIST);
  write_file("/sub/f.txt", "/sub");
  write_file("/sub/x/f.txt", "/sub/x");
  check_cwd("/sub");

  // `chdir` is relative to the initial cwd.
  TEST(chdir("x") == 0);
  check_cwd("/sub/x");

  // Resetting, as done before snapshotting, goes back to the initial cwd.
  __wasilibc_reset_preopens();
  check_cwd("/sub");

  return t_status;
}
