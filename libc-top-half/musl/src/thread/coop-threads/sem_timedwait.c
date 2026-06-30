#include "pthread_impl.h"
#include <limits.h>
#include <semaphore.h>

int sem_timedwait(sem_t *restrict sem, const struct timespec *restrict at) {
  pthread_testcancel();

  while (sem->__count <= 0) {
    /* Would need to block, but timeouts aren't supported in cooperative
     * threading: there's no way to interrupt a suspended thread after a
     * timeout. */
    if (at) {
      errno = ENOSYS;
      return -1;
    }

    __waitlist_wait_on(&sem->__waiters);
  }

  sem->__count--;
  return 0;
}
