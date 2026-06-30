#include <errno.h>
#include <pthread.h>

int pthread_setconcurrency(int val) {
  if (val < 0)
    return EINVAL;
  if (val > 0)
    return EAGAIN;
  return 0;
}
