#include "test.h"
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
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

// `argv[1]` is the expected initial cwd, either `/` or `/sub`. Each of those
// directories gets an `f.txt` containing the directory's name, so reading
// `f.txt` shows which one relative paths resolve against.
int main(int argc, char **argv) {
  TEST(argc == 2);
  const char *expected = argv[1];

  TEST(mkdir("/sub", 0755) == 0 || errno == EEXIST);
  write_file("/f.txt", "/");
  write_file("/sub/f.txt", "/sub");

  char buf[16] = {0};
  int fd = open("f.txt", O_RDONLY);
  TEST(fd >= 0);
  TEST(read(fd, buf, sizeof(buf) - 1) >= 0);
  TEST(close(fd) == 0);
  TEST(strcmp(buf, expected) == 0);

#ifdef TEST_GETCWD
  // `getcwd` is only checked by `initial_cwd_getcwd.c` so that this test checks
  // that relative paths honour the initial cwd even if `getcwd` and `chdir`
  // aren't linked.
  char cwd[16];
  TEST(getcwd(cwd, sizeof(cwd)) != NULL);
  TEST(strcmp(cwd, expected) == 0);
#endif

  return t_status;
}
