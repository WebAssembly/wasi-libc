#include "test.h"
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define TEST(c)                                                                \
  do {                                                                         \
    errno = 0;                                                                 \
    if (!(c))                                                                  \
      t_error("%s failed (errno = %d)\n", #c, errno);                          \
  } while (0)

static void test_roundtrip(void) {
  int fds[2];
  TEST(pipe(fds) == 0);
  TEST(fds[0] >= 0);
  TEST(fds[1] >= 0);
  TEST(fds[0] != fds[1]);

  char buf[16];
  TEST(write(fds[1], "hello", 5) == 5);
  TEST(read(fds[0], buf, sizeof(buf)) == 5);
  TEST(memcmp(buf, "hello", 5) == 0);

  // Partial reads drain the buffer incrementally.
  TEST(write(fds[1], "abcdefghij", 10) == 10);
  TEST(read(fds[0], buf, 4) == 4);
  TEST(memcmp(buf, "abcd", 4) == 0);
  TEST(read(fds[0], buf, sizeof(buf)) == 6);
  TEST(memcmp(buf, "efghij", 6) == 0);

  // Two writes may be coalesced into one read.
  TEST(write(fds[1], "xy", 2) == 2);
  TEST(write(fds[1], "z", 1) == 1);
  TEST(read(fds[0], buf, sizeof(buf)) == 3);
  TEST(memcmp(buf, "xyz", 3) == 0);

  TEST(close(fds[0]) == 0);
  TEST(close(fds[1]) == 0);
}

static void test_wrong_direction(void) {
  int fds[2];
  TEST(pipe(fds) == 0);

  char c = 'x';
  TEST(write(fds[0], &c, 1) == -1 && errno == EBADF);
  TEST(read(fds[1], &c, 1) == -1 && errno == EBADF);

  TEST(close(fds[0]) == 0);
  TEST(close(fds[1]) == 0);
}

static void test_eof(void) {
  int fds[2];
  TEST(pipe(fds) == 0);

  // Buffered data survives closing the write end; EOF comes after.
  TEST(write(fds[1], "tail", 4) == 4);
  TEST(close(fds[1]) == 0);
  char buf[16];
  TEST(read(fds[0], buf, sizeof(buf)) == 4);
  TEST(memcmp(buf, "tail", 4) == 0);
  TEST(read(fds[0], buf, sizeof(buf)) == 0);
  TEST(read(fds[0], buf, sizeof(buf)) == 0);
  TEST(close(fds[0]) == 0);
}

static void test_epipe(void) {
  int fds[2];
  TEST(pipe(fds) == 0);
  TEST(close(fds[0]) == 0);
  TEST(write(fds[1], "x", 1) == -1 && errno == EPIPE);
  TEST(close(fds[1]) == 0);
}

static void test_nonblocking(void) {
  int fds[2];
  TEST(pipe2(fds, O_NONBLOCK) == 0);

  char buf[16];
  TEST(read(fds[0], buf, sizeof(buf)) == -1 && errno == EAGAIN);

  // Fill the pipe to capacity, which is expected to be at least
  // _POSIX_PIPE_BUF. Cap the loop to avoid spinning forever if EAGAIN is
  // never returned.
  static char big[64 * 1024];
  memset(big, 'a', sizeof(big));
  size_t total = 0;
  for (int i = 0; i < 1000; i++) {
    ssize_t n = write(fds[1], big, sizeof(big));
    if (n == -1) {
      int err = errno;
      TEST(err == EAGAIN);
      break;
    }
    TEST(n > 0);
    total += n;
  }
  TEST(total >= _POSIX_PIPE_BUF);

  // Still EAGAIN when full.
  TEST(write(fds[1], "x", 1) == -1 && errno == EAGAIN);

  // Drain it all back out.
  size_t drained = 0;
  for (int i = 0; i < 1000; i++) {
    ssize_t n = read(fds[0], big, sizeof(big));
    if (n == -1) {
      int err = errno;
      TEST(err == EAGAIN);
      break;
    }
    TEST(n > 0);
    drained += n;
  }
  TEST(drained == total);

  TEST(close(fds[0]) == 0);
  TEST(close(fds[1]) == 0);
}

static void test_fcntl_flags(void) {
  int fds[2];
  TEST(pipe(fds) == 0);

  int fl = fcntl(fds[0], F_GETFL);
  TEST(fl >= 0 && (fl & O_ACCMODE) == O_RDONLY);
  TEST((fl & O_NONBLOCK) == 0);
  fl = fcntl(fds[1], F_GETFL);
  TEST(fl >= 0 && (fl & O_ACCMODE) == O_WRONLY);
  TEST((fl & O_NONBLOCK) == 0);

  // Toggling O_NONBLOCK on an end is reflected in its own F_GETFL.
  TEST(fcntl(fds[0], F_SETFL, O_NONBLOCK) == 0);
  fl = fcntl(fds[0], F_GETFL);
  TEST(fl >= 0 && (fl & O_NONBLOCK) != 0);
  TEST(fcntl(fds[0], F_SETFL, 0) == 0);
  fl = fcntl(fds[0], F_GETFL);
  TEST(fl >= 0 && (fl & O_NONBLOCK) == 0);

  TEST(close(fds[0]) == 0);
  TEST(close(fds[1]) == 0);

  // O_NONBLOCK requested at creation is reported by F_GETFL.
  TEST(pipe2(fds, O_NONBLOCK) == 0);
  fl = fcntl(fds[0], F_GETFL);
  TEST(fl >= 0 && (fl & O_NONBLOCK) != 0);
  TEST(close(fds[0]) == 0);
  TEST(close(fds[1]) == 0);

  // O_CLOEXEC is accepted, unsupported flags are rejected.
  TEST(pipe2(fds, O_CLOEXEC) == 0);
  TEST(close(fds[0]) == 0);
  TEST(close(fds[1]) == 0);
  TEST(pipe2(fds, O_APPEND) == -1 && errno == EINVAL);
}

static void test_stdio(void) {
  int fds[2];
  TEST(pipe(fds) == 0);

  FILE *w = fdopen(fds[1], "w");
  TEST(w != NULL);
  TEST(fputs("hello\n", w) >= 0);
  TEST(fclose(w) == 0);

  FILE *r = fdopen(fds[0], "r");
  TEST(r != NULL);
  char buf[32];
  TEST(fgets(buf, sizeof(buf), r) != NULL);
  TEST(strcmp(buf, "hello\n") == 0);
  TEST(fgets(buf, sizeof(buf), r) == NULL);
  TEST(feof(r));
  TEST(fclose(r) == 0);
}

int main(void) {
#if defined(__wasip1__) || defined(__wasip2__)
  if (1)
    return t_status;
#endif
  test_roundtrip();
  test_wrong_direction();
  test_eof();
  test_epipe();
  test_nonblocking();
  test_fcntl_flags();
  test_stdio();
  return t_status;
}
