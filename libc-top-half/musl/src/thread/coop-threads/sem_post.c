#include "pthread_impl.h"
#include <limits.h>
#include <semaphore.h>

int sem_post(sem_t *sem) {
  if (sem->__val[0] == SEM_VALUE_MAX) {
    errno = EOVERFLOW;
    return -1;
  }

  /* Make a permit available and wake one waiter sleeping on the futex token.
   * The woken waiter re-checks the count and consumes the permit. */
  sem->__val[0]++;
  __wake(&sem->__val[1], 1, 1);

  return 0;
}
