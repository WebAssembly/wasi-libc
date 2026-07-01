#include "pthread_impl.h"

int pthread_setschedparam(pthread_t t, int policy,
                          const struct sched_param *param) {
  return ENOTSUP;
}
