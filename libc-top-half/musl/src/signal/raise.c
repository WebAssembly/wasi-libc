#include <signal.h>
#include <stdint.h>
#ifdef __wasilibc_unmodified_upstream
#include "syscall.h"
#else
#include <wasi/api.h>
#endif
#include "pthread_impl.h"

int raise(int sig)
{
	sigset_t set;
#ifdef __wasilibc_unmodified_upstream
	__block_app_sigs(&set);
#endif
#ifdef __wasilibc_unmodified_upstream
	int ret = syscall(SYS_tkill, __pthread_self()->tid, sig);
#else
	int ret = __wasi_thread_signal(__pthread_self()->tid, (__wasi_signal_t)sig);
#endif
#ifdef __wasilibc_unmodified_upstream
	__restore_sigs(&set);
#endif
	return ret;
}