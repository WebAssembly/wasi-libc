#include "pthread_impl.h"
#include <limits.h>
#include <semaphore.h>
#include <time.h>

int sem_timedwait(sem_t *restrict sem, const struct timespec *restrict at) {
  pthread_testcancel();

  while (sem->__val[0] <= 0) {
    int rc = __timedwait(&sem->__val[1], 1, CLOCK_REALTIME, at, 0);
    if (rc != 0) {
      errno = rc;
      return -1;
    }
  }

  sem->__val[0]--;
  return 0;
}
