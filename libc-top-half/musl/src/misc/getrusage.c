#include <sys/resource.h>
#include <string.h>
#include <errno.h>
#ifdef __wasilibc_unmodified_upstream
#include "syscall.h"
#else
#include <__typedef_clock_t.h>
#include <__struct_rusage.h>
clock_t __clock(void);
#endif

int getrusage(int who, struct rusage *ru)
{
#ifdef __wasilibc_unmodified_upstream
	int r;
#ifdef SYS_getrusage_time64
	long long kru64[18];
	r = __syscall(SYS_getrusage_time64, who, kru64);
	if (!r) {
		ru->ru_utime = (struct timeval)
			{ .tv_sec = kru64[0], .tv_usec = kru64[1] };
		ru->ru_stime = (struct timeval)
			{ .tv_sec = kru64[2], .tv_usec = kru64[3] };
		char *slots = (char *)&ru->ru_maxrss;
		for (int i=0; i<14; i++)
			*(long *)(slots + i*sizeof(long)) = kru64[4+i];
	}
	if (SYS_getrusage_time64 == SYS_getrusage || r != -ENOSYS)
		return __syscall_ret(r);
#endif
	char *dest = (char *)&ru->ru_maxrss - 4*sizeof(long);
	r = __syscall(SYS_getrusage, who, dest);
	if (!r && sizeof(time_t) > sizeof(long)) {
		long kru[4];
		memcpy(kru, dest, 4*sizeof(long));
		ru->ru_utime = (struct timeval)
			{ .tv_sec = kru[0], .tv_usec = kru[1] };
		ru->ru_stime = (struct timeval)
			{ .tv_sec = kru[2], .tv_usec = kru[3] };
	}
	return __syscall_ret(r);
#else
	switch (who) {
		case RUSAGE_SELF: {
			__wasi_timestamp_t now = 0;
    		(void)__wasi_clock_time_get(__WASI_CLOCKID_MONOTONIC, 0, &now);

			ru->ru_utime.tv_sec = now / 1000000000;
			ru->ru_utime.tv_usec = (now % 1000000000) * 1000;
			ru->ru_stime = ru->ru_utime;
			return 0;
		}
		case RUSAGE_CHILDREN:
			*ru = (struct rusage) {};
			return 0;
		default:
			errno = EINVAL;
			return -1;
	}
#endif
}
