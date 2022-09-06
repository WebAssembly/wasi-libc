#include <sys/times.h>
#ifdef __wasilibc_unmodified_upstream
#include "syscall.h"
#else
#include <wasi/api.h>
#endif
#include <errno.h>

#define NSEC_PER_SEC 1000000000

clock_t times(struct tms *tms)
{
#ifdef __wasilibc_unmodified_upstream
	return __syscall(SYS_times, tms);
#else
	__wasi_timestamp_t ts;
	__wasi_errno_t error = __wasi_clock_time_get(__WASI_CLOCKID_REALTIME, 1, &ts);
	if (error != 0) {
		errno = error;
		return -1;
	}
	tms->tms_stime = ts / NSEC_PER_SEC;
	tms->tms_utime = (ts % NSEC_PER_SEC) / 1000;
	
	__wasi_timestamp_t cts;
	error = __wasi_clock_time_get(__WASI_CLOCKID_PROCESS_CPUTIME_ID, 1, &cts);
	if (error != 0) {
		tms->tms_cstime = tms->tms_stime;
		tms->tms_cutime = tms->tms_utime;
	} else {
		tms->tms_cstime = ts / NSEC_PER_SEC;
		tms->tms_cutime = (ts % NSEC_PER_SEC) / 1000;
	}
	return ts;
#endif
}
