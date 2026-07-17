#include "pthread_impl.h"
#include <limits.h>
#include <semaphore.h>

int sem_trywait(sem_t *sem) {
  if (sem->__val[0] > 0) {
    sem->__val[0]--;
    return 0;
  }
  errno = EAGAIN;
  return -1;
}
