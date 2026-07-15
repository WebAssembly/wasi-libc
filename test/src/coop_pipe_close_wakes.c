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
  TEST(read(fds[0], buf, sizeof(buf)) == 0);
  return NULL;
}

static void *blocked_writer(void *arg) {
  (void)arg;
  errno = 0;
  ssize_t n = write(fds[1], "x", 1);
  int err = errno;
  TEST(n == -1 && err == EPIPE);
  return NULL;
}

int main(void) {
  // Closing the write end wakes a blocked reader with EOF.
  TEST(pipe(fds) == 0);
  pthread_t t;
  TEST(pthread_create(&t, NULL, blocked_reader, NULL) == 0);
  for (int i = 0; i < 10; i++)
    TEST(sched_yield() == 0);
  TEST(close(fds[1]) == 0);
  TEST(pthread_join(t, NULL) == 0);
  TEST(close(fds[0]) == 0);

  // Closing the read end wakes a blocked writer with EPIPE.
  TEST(pipe(fds) == 0);
  TEST(fcntl(fds[1], F_SETFL, O_NONBLOCK) == 0);
  static char big[8192];
  memset(big, 'a', sizeof(big));
  for (int i = 0; i < 1000; i++) {
    if (write(fds[1], big, sizeof(big)) == -1) {
      int err = errno;
      TEST(err == EAGAIN);
      break;
    }
  }
  TEST(fcntl(fds[1], F_SETFL, 0) == 0);
  TEST(pthread_create(&t, NULL, blocked_writer, NULL) == 0);
  for (int i = 0; i < 10; i++)
    TEST(sched_yield() == 0);
  TEST(close(fds[0]) == 0);
  TEST(pthread_join(t, NULL) == 0);
  TEST(close(fds[1]) == 0);

  return t_status;
}
