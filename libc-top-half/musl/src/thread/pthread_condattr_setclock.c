#include "pthread_impl.h"

#ifndef __wasilibc_unmodified_upstream
#include <common/clock.h>
#endif

int pthread_condattr_setclock(pthread_condattr_t *a, clockid_t clk)
{
	if (clk < 0 || clk-2U < 2) return EINVAL;
	a->__attr &= 0x80000000;
	a->__attr |= clk;
	return 0;
}
