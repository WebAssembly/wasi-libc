#include "test.h"
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <wasi/libc-find-relpath.h>

#define TEST(c)                                                                \
  do {                                                                         \
    errno = 0;                                                                 \
    if (!(c))                                                                  \
      t_error("%s failed (errno = %d)\n", #c, errno);                          \
  } while (0)

// Writes `contents` to `path`, which must be absolute.
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

// Checks that the public `__wasilibc_find_relpath` resolves `f.txt` against
// the cwd `expected`, failing with `ERANGE` if the buffer is too small for the
// relative path.
static void check_find_relpath(const char *expected) {
  // The test's directory is preopened at `/`, so the relative path is the
  // absolute path without the leading `/`.
  char want[32];
  if (strcmp(expected, "/") == 0)
    snprintf(want, sizeof(want), "f.txt");
  else
    snprintf(want, sizeof(want), "%s/f.txt", expected + 1);

  const char *abs = NULL;
  char small[2];
  char *rel = small;
  TEST(__wasilibc_find_relpath("f.txt", &abs, &rel, sizeof(small)) == -1 &&
       errno == ERANGE);

  // Retry with just enough room, as callers are expected to.
  char exact[32];
  rel = exact;
  int dirfd = __wasilibc_find_relpath("f.txt", &abs, &rel, strlen(want) + 1);
  TEST(dirfd >= 0);
  TEST(strcmp(abs, "") == 0);
  TEST(strcmp(rel, want) == 0);

  char buf[16] = {0};
  int fd = openat(dirfd, rel, O_RDONLY);
  TEST(fd >= 0);
  TEST(read(fd, buf, sizeof(buf) - 1) >= 0);
  TEST(close(fd) == 0);
  TEST(strcmp(buf, expected) == 0);
}

// `argv[1]` is the expected initial cwd, either `/` or `/sub`. Each of those
// directories gets an `f.txt` containing the directory's name, so reading
// `f.txt` shows which one relative paths resolve against.
//
// This test doesn't use `chdir`, so it checks that path lookup uses the initial
// cwd even when `chdir` isn't linked.
int main(int argc, char **argv) {
  TEST(argc == 2);
  const char *expected = argv[1];

  TEST(mkdir("/sub", 0755) == 0 || errno == EEXIST);
  write_file("/f.txt", "/");
  write_file("/sub/f.txt", "/sub");
  check_cwd(expected);
  check_find_relpath(expected);

  return t_status;
}
