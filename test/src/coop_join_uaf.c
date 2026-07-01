#include "test.h"
#include <pthread.h>
#include <semaphore.h>
#include <stdint.h>
#include <string.h>

// Stress test to ensure that when a thread exits while another thread is
// join-ing everything works out. Right now pthread stacks are malloc'd and so
// the brief window of time when a thread is exiting and something else is
// joining it shouldn't result in a UAF.

#define TEST(c)                                                                \
  do {                                                                         \
    if (!(c))                                                                  \
      t_error("%s failed\n", #c);                                              \
  } while (0)

#define STACK_SIZE (64 * 1024)

static sem_t go;
static sem_t never;
static pthread_t t1;

static void *blocker(void *arg) {
  (void)arg;
  TEST(sem_wait(&go) == 0);
  return (void *)0x1234;
}

// Helper thread to, in buggy situations, smash the stack that `blocker` was
// previously using if it releases its stack too soon.
static void *stack_smasher(void *arg) {
  (void)arg;
  volatile unsigned char buf[40 * 1024];
  memset((void *)buf, 0xAA, sizeof buf);
  __asm__ volatile("" ::: "memory");
  sem_wait(&never); // never returns
  __builtin_trap();
}

static void *joiner(void *arg) {
  (void)arg;
  void *ret = NULL;

  // Block on the result of the `blocker` thread. This'll deallocate its stack
  // and then we're going to smash the stack a bit below in an attempt to ensure
  // that the `blocker` thread isn't actually still running lest it run into
  // issues.
  TEST(pthread_join(t1, &ret) == 0);
  TEST(ret == (void *)0x1234);

  // Spin up the `stack_smasher` thread to grab the stack `blocker` is using and
  // poison it to help expose possible issues should one exist.
  pthread_attr_t attr;
  TEST(pthread_attr_init(&attr) == 0);
  TEST(pthread_attr_setstacksize(&attr, STACK_SIZE) == 0);
  pthread_t t3;
  TEST(pthread_create(&t3, &attr, stack_smasher, NULL) == 0);
  TEST(pthread_attr_destroy(&attr) == 0);
  return NULL;
}

int main(void) {
  TEST(sem_init(&go, 0, 0) == 0);
  TEST(sem_init(&never, 0, 0) == 0);

  // Start our candidate thread, `blocker`, which'll block on the `go`
  // semaphore.
  pthread_attr_t attr;
  TEST(pthread_attr_init(&attr) == 0);
  TEST(pthread_attr_setstacksize(&attr, STACK_SIZE) == 0);
  TEST(pthread_create(&t1, &attr, blocker, NULL) == 0);
  TEST(pthread_attr_destroy(&attr) == 0);

  // Spin up a thread to wait on `blocker`. This should immediately switch to it
  // which will then cause it to block waiting for the thread back to here.
  pthread_t t2;
  TEST(pthread_create(&t2, NULL, joiner, NULL) == 0);

  // Let everything run now...
  TEST(sem_post(&go) == 0);

  // And finally the `joiner` thread shouldn't have any issues...
  TEST(pthread_join(t2, NULL) == 0);

  // ... and if we've gotten this far no traps/oob/uaf hopefully happened, so
  // yay!

  return t_status;
}
