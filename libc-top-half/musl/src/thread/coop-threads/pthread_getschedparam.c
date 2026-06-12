#include "pthread_impl.h"

int pthread_getschedparam(pthread_t t, int *restrict policy, struct sched_param *restrict param)
{
    return ENOTSUP;
}