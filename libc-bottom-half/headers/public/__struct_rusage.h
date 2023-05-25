#ifndef __wasilibc___struct_rusage_h
#define __wasilibc___struct_rusage_h

#include <__struct_timeval.h>

/* TODO: Add more features here. */
struct rusage {
	struct timeval ru_utime;
	struct timeval ru_stime;
	/* linux extensions; not used in WASI yet, but necessary for backwards
	 * compatibility purposes. */
	long	ru_maxrss;
	long	ru_ixrss;
	long	ru_idrss;
	long	ru_isrss;
	long	ru_minflt;
	long	ru_majflt;
	long	ru_nswap;
	long	ru_inblock;
	long	ru_oublock;
	long	ru_msgsnd;
	long	ru_msgrcv;
	long	ru_nsignals;
	long	ru_nvcsw;
	long	ru_nivcsw;
	/* room for more... */
	long    __reserved[16];
};

#endif
