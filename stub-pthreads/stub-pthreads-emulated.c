// This file is linked into wasi-emulated-pthread

#include "pthread_impl.h"

int __pthread_create(pthread_t *restrict res, const pthread_attr_t *restrict attrp, void *(*entry)(void *), void *restrict arg)
{
	/*
		"The system lacked the necessary resources to create another thread,
		or the system-imposed limit on the total number of threads in a process
		{PTHREAD_THREADS_MAX} would be exceeded."
	*/
	return EAGAIN;
}
weak_alias(__pthread_create, pthread_create);
int __pthread_detach(pthread_t t)
{
	/*
		If we are the only thread, when we exit the whole process exits.
		So the storage will be reclaimed no matter what.
	*/
	return 0;
}
weak_alias(__pthread_detach, pthread_detach);
int __pthread_join(pthread_t t, void **res)
{
	/*
		"The behavior is undefined if the value specified by the thread argument
		to pthread_join() refers to the calling thread."
	*/
	return 0;
}
weak_alias(__pthread_join, pthread_join);
int pthread_tryjoin_np(pthread_t t, void **res)
{
	return 0;
}
int pthread_timedjoin_np(pthread_t t, void **res, const struct timespec *at)
{
	return 0;
}
