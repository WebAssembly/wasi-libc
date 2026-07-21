#include "pthread_impl.h"
#ifndef __wasilibc_unmodified_upstream
#include "assert.h"
#endif

#include <wasi/libc.h>

void __wait(volatile int *addr, volatile int *waiters, int val, int priv)
{
	int spins=100;
	if (priv) priv = FUTEX_PRIVATE;
	while (spins-- && (!waiters || !*waiters)) {
		if (*addr==val) a_spin();
		else return;
	}
	if (waiters) a_inc(waiters);
	while (*addr==val) {
#ifdef __wasilibc_unmodified_upstream
		__syscall(SYS_futex, addr, FUTEX_WAIT|priv, val, 0) != -ENOSYS
		|| __syscall(SYS_futex, addr, FUTEX_WAIT, val, 0);
#else
                __wasilibc_futex_wait(addr, val, 0, NULL, 0);
#endif
	}
	if (waiters) a_dec(waiters);
}

void __wake(volatile void *addr, int cnt, int priv)
{
	if (priv) priv = FUTEX_PRIVATE;
	if (cnt<0) cnt = INT_MAX;
#ifdef __wasilibc_unmodified_upstream
	__syscall(SYS_futex, addr, FUTEX_WAKE|priv, cnt) != -ENOSYS ||
	__syscall(SYS_futex, addr, FUTEX_WAKE, cnt);
#else
        __wasilibc_futex_wake(addr, cnt, 0);
#endif
}

void __futexwait(volatile void *addr, int val, int priv)
{
#ifdef __wasilibc_unmodified_upstream
	if (priv) priv = FUTEX_PRIVATE;
	__syscall(SYS_futex, addr, FUTEX_WAIT|priv, val, 0) != -ENOSYS ||
	__syscall(SYS_futex, addr, FUTEX_WAIT, val, 0);
#else
	__wait(addr, NULL, val, priv);
#endif
}
