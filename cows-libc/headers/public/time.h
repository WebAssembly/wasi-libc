#ifndef __cowslibc_time_h
#define __cowslibc_time_h

#define __need_size_t
#define __need_NULL
#include <stddef.h>

#include <__typedef_time_t.h>
#include <__struct_timespec.h>
#include <__struct_tm.h>
#include <__typedef_clockid_t.h>

#include <cloudabi_types_common.h>

#define TIMER_ABSTIME CLOUDABI_SUBSCRIPTION_CLOCK_ABSTIME

// FIXME: POSIX requires this to be 1000000, and that's what glibc and musl use.
#define CLOCKS_PER_SEC 1000000000

#define TIME_UTC 1

#endif
