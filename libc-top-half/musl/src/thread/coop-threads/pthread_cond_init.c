#include "pthread_impl.h"

int pthread_cond_init(pthread_cond_t *restrict c,
                      const pthread_condattr_t *restrict a) {
  *c = (pthread_cond_t){0};
  return 0;
}
