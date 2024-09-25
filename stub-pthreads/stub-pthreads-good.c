// This file is linked into libc

#include "pthread_impl.h"

static void dummy_0()
{
}
weak_alias(dummy_0, __acquire_ptc);
weak_alias(dummy_0, __release_ptc);

int pthread_once(pthread_once_t *control, void (*init)(void))
{
	if (!*control) {
		init();
		*control = 1;
	}
	return 0;
}
