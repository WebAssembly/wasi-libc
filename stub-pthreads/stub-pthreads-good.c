// This file is linked into libc

#include "pthread_impl.h"

static void dummy_0()
{
}
weak_alias(dummy_0, __acquire_ptc);
weak_alias(dummy_0, __release_ptc);
weak_alias(dummy_0, __pthread_tsd_run_dtors);

int pthread_once(pthread_once_t *control, void (*init)(void))
{
	if (!*control) {
		init();
		*control = 1;
	}
	return 0;
}

_Noreturn void pthread_exit(void *result)
{
	/*
		We are the only thread, so when we exit the whole process exits.
		But we still have to run cancellation handlers...
	*/
	pthread_t self = __pthread_self();

	self->canceldisable = 1;
	self->cancelasync = 0;
	self->result = result;

	while (self->cancelbuf) {
		void (*f)(void *) = self->cancelbuf->__f;
		void *x = self->cancelbuf->__x;
		self->cancelbuf = self->cancelbuf->__next;
		f(x);
	}

	__pthread_tsd_run_dtors();

	exit(0);
}
