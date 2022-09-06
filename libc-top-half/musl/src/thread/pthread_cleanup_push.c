#include "pthread_impl.h"

#ifdef __wasilibc_unmodified_upstream
static void dummy(struct __ptcb *cb)
{
}
weak_alias(dummy, __do_cleanup_push);
weak_alias(dummy, __do_cleanup_pop);
#endif

void _pthread_cleanup_push(struct __ptcb *cb, void (*f)(void *), void *x)
{
	cb->__f = f;
	cb->__x = x;
	__do_cleanup_push(cb);
}

void _pthread_cleanup_pop(struct __ptcb *cb, int run)
{
	__do_cleanup_pop(cb);
	if (run) cb->__f(cb->__x);
}
