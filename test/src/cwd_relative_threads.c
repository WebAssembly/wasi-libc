#include "test.h"
#include <fcntl.h>
#include <pthread.h>
#include <unistd.h>

#define TEST(c)                                                                \
  do {                                                                         \
    if (!(c))                                                                  \
      t_error("%s failed\n", #c);                                              \
  } while (0)

static void *worker(void *arg) {
  (void)arg;
  int fd = open("some-relative-path", O_RDONLY);
  if (fd >= 0)
    close(fd);
  return NULL;
}

int main(void) {
  TEST(chdir("/") == 0);

  pthread_t t;
  if (pthread_create(&t, NULL, worker, NULL) == 0) {
    TEST(pthread_join(t, NULL) == 0);
  } else {
    worker(NULL);
  }
  return t_status;
}
