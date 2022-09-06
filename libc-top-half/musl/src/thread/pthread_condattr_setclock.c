#include "pthread_impl.h"

int pthread_condattr_setclock(pthread_condattr_t *a, clockid_t clk)
{
#ifdef __wasilibc_unmodified_upstream
	if (clk < 0 || clk-2U < 2) return EINVAL;
	a->__attr &= 0x80000000;
	a->__attr |= clk;
#else
    if (clk.id < 0 || clk.id-2U < 2) return EINVAL;
	a->__attr &= 0x80000000;
	a->__attr |= clk.id;
#endif
	return 0;
}