#include "pthread_impl.h"
#include <limits.h>
#include <semaphore.h>

int sem_post(sem_t *sem) {
  if (sem->__count == SEM_VALUE_MAX) {
    errno = EOVERFLOW;
    return -1;
  }

  /* Make a permit available and wake a waiter if there is one. The woken
   * waiter re-checks the count and consumes the permit; `__waitlist_wake_one`
   * is a no-op when nobody is waiting. */
  sem->__count++;
  __waitlist_wake_one(&sem->__waiters, 1);

  return 0;
}
