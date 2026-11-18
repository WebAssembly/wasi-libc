#include "test.h"
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

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
// resolves to the one in `dir`.
static void check_cwd(const char *expected, const char *dir) {
  char cwd[16];
  TEST(getcwd(cwd, sizeof(cwd)) != NULL);
  TEST(strcmp(cwd, expected) == 0);

  char buf[16] = {0};
  int fd = open("f.txt", O_RDONLY);
  TEST(fd >= 0);
  TEST(read(fd, buf, sizeof(buf) - 1) >= 0);
  TEST(close(fd) == 0);
  TEST(strcmp(buf, dir) == 0);
}

// `argv[1]` is the expected initial cwd, and `argv[2]`, if given, is the
// directory that relative paths resolve against, either `/` or `/sub`, which
// defaults to `argv[1]`. Each of those directories gets an `f.txt` containing
// the directory's name, so reading `f.txt` shows which one relative paths
// resolve against.
//
// This test doesn't use `chdir`, so it checks that path lookup uses the initial
// cwd even when `chdir` isn't linked.
int main(int argc, char **argv) {
  TEST(argc == 2 || argc == 3);
  const char *expected = argv[1];
  const char *dir = argc == 3 ? argv[2] : expected;

  TEST(mkdir("/sub", 0755) == 0 || errno == EEXIST);
  write_file("/f.txt", "/");
  write_file("/sub/f.txt", "/sub");
  check_cwd(expected, dir);

  return t_status;
}
