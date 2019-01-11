#ifndef __cowslibc___struct_rusage_h
#define __cowslibc___struct_rusage_h

#include "__struct_timeval.h"

/* TODO: Add more features here. */
struct rusage {
    struct timeval ru_utime;
    struct timeval ru_stime;
};

#endif
