#define _GNU_SOURCE
#include "test.h"
#include <errno.h>
#include <pthread.h>
#include <semaphore.h>
#include <string.h>

#define TEST(c)                                                                \
  do {                                                                         \
    if (!(c))                                                                  \
      t_error("%s failed\n", #c);                                              \
  } while (0)

static sem_t go;
static sem_t named;
static sem_t checked;

static void *worker(void *arg) {
  (void)arg;
  char buf[16];

  // The name set by the main thread is visible here.
  TEST(sem_wait(&go) == 0);
  TEST(pthread_getname_np(pthread_self(), buf, sizeof(buf)) == 0);
  TEST(strcmp(buf, "from-main") == 0);

  // Rename ourselves and let the main thread observe it.
  TEST(pthread_setname_np(pthread_self(), "from-worker") == 0);
  TEST(sem_post(&named) == 0);
  TEST(sem_wait(&checked) == 0);
  return NULL;
}

int main(void) {
  char buf[32];

  // The main thread starts out with no name.
  TEST(pthread_getname_np(pthread_self(), buf, sizeof(buf)) == 0);
  TEST(strcmp(buf, "") == 0);

  TEST(pthread_setname_np(pthread_self(), "main-thread") == 0);
  TEST(pthread_getname_np(pthread_self(), buf, sizeof(buf)) == 0);
  TEST(strcmp(buf, "main-thread") == 0);

  // size limits
  TEST(pthread_setname_np(pthread_self(), "123456789012345") == 0);
  TEST(pthread_getname_np(pthread_self(), buf, sizeof(buf)) == 0);
  TEST(strcmp(buf, "123456789012345") == 0);
  TEST(pthread_setname_np(pthread_self(), "1234567890123456") == ERANGE);

  char small[4];
  TEST(pthread_getname_np(pthread_self(), small, sizeof(small)) == ERANGE);
  char exact[16];
  TEST(pthread_getname_np(pthread_self(), exact, sizeof(exact)) == 0);
  TEST(strcmp(exact, "123456789012345") == 0);

  // Names can be set and read across threads.
  TEST(sem_init(&go, 0, 0) == 0);
  TEST(sem_init(&named, 0, 0) == 0);
  TEST(sem_init(&checked, 0, 0) == 0);
  pthread_t t;
  pthread_attr_t attr;
  TEST(pthread_attr_init(&attr) == 0);
  TEST(pthread_create(&t, &attr, worker, NULL) == 0);
  TEST(pthread_setname_np(t, "from-main") == 0);
  TEST(sem_post(&go) == 0);

  TEST(sem_wait(&named) == 0);
  TEST(pthread_getname_np(t, buf, sizeof(buf)) == 0);
  TEST(strcmp(buf, "from-worker") == 0);
  TEST(sem_post(&checked) == 0);

  TEST(pthread_join(t, NULL) == 0);
  return t_status;
}
