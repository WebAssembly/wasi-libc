#include "pthread_impl.h"
#include <limits.h>
#include <semaphore.h>

int sem_trywait(sem_t *sem) {
  if (sem->__count > 0) {
    sem->__count--;
    return 0;
  }
  errno = EAGAIN;
  return -1;
}
