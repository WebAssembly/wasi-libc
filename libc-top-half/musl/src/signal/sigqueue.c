#include <signal.h>
#include <string.h>
#include <unistd.h>
#ifdef __wasilibc_unmodified_upstream
#include "syscall.h"
#endif
#include "pthread_impl.h"

int sigqueue(pid_t pid, int sig, const union sigval value)
{
	siginfo_t si;
	sigset_t set;
	int r;
	memset(&si, 0, sizeof si);
	si.si_signo = sig;
	si.si_code = SI_QUEUE;
	si.si_value = value;
	si.si_uid = getuid();
#ifdef __wasilibc_unmodified_upstream
	__block_app_sigs(&set);
#endif
	si.si_pid = getpid();
#ifdef __wasilibc_unmodified_upstream
	r = syscall(SYS_rt_sigqueueinfo, pid, sig, &si);
#else
    r = EINVAL;
#endif
#ifdef __wasilibc_unmodified_upstream
	__restore_sigs(&set);
#endif
	return r;
}
