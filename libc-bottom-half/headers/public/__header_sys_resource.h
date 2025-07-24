#ifndef __wasilibc___header_sys_resource_h
#define __wasilibc___header_sys_resource_h

#include <__struct_rusage.h>

#define RUSAGE_SELF 1
#define RUSAGE_CHILDREN 2

// TODO: We don't support any of these yet, but they are defined for compatibility.
#define RLIMIT_CPU     0
#define RLIMIT_FSIZE   1
#define RLIMIT_DATA    2
#define RLIMIT_STACK   3
#define RLIMIT_CORE    4
#ifndef RLIMIT_RSS
#define RLIMIT_RSS     5
#define RLIMIT_NPROC   6
#define RLIMIT_NOFILE  7
#define RLIMIT_MEMLOCK 8
#define RLIMIT_AS      9
#endif
#define RLIMIT_LOCKS   10
#define RLIMIT_SIGPENDING 11
#define RLIMIT_MSGQUEUE 12
#define RLIMIT_NICE    13
#define RLIMIT_RTPRIO  14
#define RLIMIT_RTTIME  15
#define RLIMIT_NLIMITS 16

#define RLIM_NLIMITS RLIMIT_NLIMITS

#ifdef __cplusplus
extern "C" {
#endif

int getrusage(int who, struct rusage *usage);

#ifdef __cplusplus
}
#endif

#endif
