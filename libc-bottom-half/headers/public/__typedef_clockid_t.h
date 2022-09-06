#ifndef __wasilibc___typedef_clockid_t_h
#define __wasilibc___typedef_clockid_t_h

#ifndef COMMON_CLOCK_H
#define COMMON_CLOCK_H
#include <wasi/api.h>

// In this implementation we define clockid_t as a pointer type, so that
// we can implement them as full objects. Right now we only use those
// objects to store the raw ABI-level clock identifier, but in the
// future we can use this to provide support for pthread_getcpuclockid()
// and clock file descriptors.
struct __clockid {
  __wasi_clockid_t id;
};
#endif

typedef struct __clockid clockid_t;

#endif
