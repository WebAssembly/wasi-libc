#include "test.h"
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <sched.h>
#include <string.h>
#include <unistd.h>

#define TEST(c)                                                                \
  do {                                                                         \
    errno = 0;                                                                 \
    if (!(c))                                                                  \
      t_error("%s failed (errno = %d)\n", #c, errno);                          \
  } while (0)

static int fds[2];

static void *blocked_reader(void *arg) {
  (void)arg;
  char buf[16];
  ssize_t n = read(fds[0], buf, sizeof(buf));
  TEST(n == 5);
  TEST(memcmp(buf, "hello", 5) == 0);
  return NULL;
}

static void *blocked_writer(void *arg) {
  (void)arg;
  TEST(write(fds[1], "world", 5) == 5);
  return NULL;
}

int main(void) {
  TEST(pipe(fds) == 0);

  // A thread blocked reading an empty pipe is woken by a write.
  pthread_t t;
  TEST(pthread_create(&t, NULL, blocked_reader, NULL) == 0);
  for (int i = 0; i < 10; i++)
    TEST(sched_yield() == 0);
  TEST(write(fds[1], "hello", 5) == 5);
  TEST(pthread_join(t, NULL) == 0);

  // Fill the pipe so the next write blocks, probing capacity with
  // O_NONBLOCK so this doesn't itself block.
  TEST(fcntl(fds[1], F_SETFL, O_NONBLOCK) == 0);
  static char big[8192];
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
  TEST(total > 0);
  TEST(fcntl(fds[1], F_SETFL, 0) == 0);

  // A thread blocked writing to a full pipe is woken by a read.
  TEST(pthread_create(&t, NULL, blocked_writer, NULL) == 0);
  for (int i = 0; i < 10; i++)
    TEST(sched_yield() == 0);
  size_t drained = 0;
  char buf[512];
  while (drained < total) {
    ssize_t n = read(fds[0], buf, sizeof(buf));
    TEST(n > 0);
    drained += n;
  }
  TEST(pthread_join(t, NULL) == 0);
  TEST(read(fds[0], buf, sizeof(buf)) == 5);
  TEST(memcmp(buf, "world", 5) == 0);

  TEST(close(fds[0]) == 0);
  TEST(close(fds[1]) == 0);
  return t_status;
}
