#include "test.h"
#include <pthread.h>
#include <stdlib.h>

#define TEST(c, i)                                                             \
  do {                                                                         \
    int ret = c;                                                               \
    if (ret != 0)                                                              \
      t_error("%s failed (i = %d, returned %d)\n", #c, i, ret);                \
  } while (0)

int ITERS = 4;

int main(void) {
  pthread_mutex_t recursive_mutex;
  pthread_mutexattr_t attr;

  // Create a recursive mutex
  pthread_mutexattr_init(&attr);
  pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
  pthread_mutex_init(&recursive_mutex, &attr);
  pthread_mutexattr_destroy(&attr);

  // Lock the mutex ITERS times
  // Deadlock can't occur since all locks are held on the same thread
  for (int i = 0; i < ITERS; i++) {
    TEST(pthread_mutex_lock(&recursive_mutex), i);
  }
  // Unlock the mutex ITERS times
  for (int i = 0; i < ITERS; i++) {
    TEST(pthread_mutex_unlock(&recursive_mutex), i);
  }

  // Lock the mutex ITERS times without blocking
  for (int i = 0; i < ITERS; i++) {
    TEST(pthread_mutex_trylock(&recursive_mutex), i);
  }
  // Unlock the mutex ITERS times again
  for (int i = 0; i < ITERS; i++) {
    TEST(pthread_mutex_unlock(&recursive_mutex), i);
  }

  pthread_mutex_destroy(&recursive_mutex);
  return t_status;
}
