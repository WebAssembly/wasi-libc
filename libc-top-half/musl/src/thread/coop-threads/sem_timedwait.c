#include "pthread_impl.h"
#include <limits.h>
#include <semaphore.h>
#include <time.h>

int sem_timedwait(sem_t *restrict sem, const struct timespec *restrict at) {
  pthread_testcancel();

  while (sem->__count <= 0) {
    int rc = __waitlist_wait_on(&sem->__waiters, CLOCK_REALTIME, at);
    if (rc != 0) {
      errno = rc;
      return -1;
    }
  }

  sem->__count--;
  return 0;
}
