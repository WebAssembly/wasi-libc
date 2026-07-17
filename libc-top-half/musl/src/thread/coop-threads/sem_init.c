#include <errno.h>
#include <limits.h>
#include <semaphore.h>
#include <stdlib.h>

int sem_init(sem_t *sem, int pshared, unsigned value) {
  if (value > SEM_VALUE_MAX) {
    errno = EINVAL;
    return -1;
  }
  sem->__val[0] = value;
  sem->__val[1] = 1;
  sem->__val[2] = pshared ? 0 : 128;
  sem->__val[3] = 0;
  return 0;
}
