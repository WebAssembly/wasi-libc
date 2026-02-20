#include "pthread_impl.h"

int pthread_cond_signal(pthread_cond_t *c)
{
	__waitlist_wake_one(&c->_c_waiters);
	return 0;
}