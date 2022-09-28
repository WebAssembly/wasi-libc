#include <unistd.h>
#include <errno.h>
#include "libc.h"
#include "lock.h"
#include "pthread_impl.h"
#include "fork_impl.h"

static volatile int *const dummy_lockptr = 0;

#ifdef __wasilibc_unmodified_upstream
weak_alias(dummy_lockptr, __at_quick_exit_lockptr);
weak_alias(dummy_lockptr, __atexit_lockptr);
weak_alias(dummy_lockptr, __locale_lockptr);
weak_alias(dummy_lockptr, __random_lockptr);
weak_alias(dummy_lockptr, __sem_open_lockptr);
weak_alias(dummy_lockptr, __stdio_ofl_lockptr);
weak_alias(dummy_lockptr, __syslog_lockptr);
weak_alias(dummy_lockptr, __timezone_lockptr);

weak_alias(dummy_lockptr, __vmlock_lockptr);
#else
extern volatile int *const __gettext_lockptr;
weak_alias(dummy_lockptr, __dlerror_lockptr);
weak_alias(dummy_lockptr, __bump_lockptr);
#endif

static volatile int *const *const atfork_locks[] = {
	&__at_quick_exit_lockptr,
	&__atexit_lockptr,
	&__dlerror_lockptr,
	&__gettext_lockptr,
	&__locale_lockptr,
	&__random_lockptr,
	&__sem_open_lockptr,
	&__stdio_ofl_lockptr,
	&__syslog_lockptr,
	&__timezone_lockptr,
	&__bump_lockptr,
};

static void dummy(int x) { }
#ifdef __wasilibc_unmodified_upstream
weak_alias(dummy, __fork_handler);
#endif
weak_alias(dummy, __malloc_atfork);
weak_alias(dummy, __ldso_atfork);

#ifdef __wasilibc_unmodified_upstream
static void dummy_0(void) { }
weak_alias(dummy_0, __tl_lock);
weak_alias(dummy_0, __tl_unlock);
#endif

pid_t fork(void)
{
	return _fork_internal(1);
}

pid_t _fork_internal(int copy_mem)
{
	sigset_t set;
	__fork_handler(-1);
#ifdef __wasilibc_unmodified_upstream
	__block_app_sigs(&set);
#endif
	int need_locks = libc.need_locks > 0;
	if (need_locks) {
		__ldso_atfork(-1);
		__inhibit_ptc();
		for (int i=0; i<sizeof atfork_locks/sizeof *atfork_locks; i++)
			if (*atfork_locks[i]) LOCK(*atfork_locks[i]);
		__malloc_atfork(-1);
		__tl_lock();
	}
	pthread_t self=__pthread_self(), next=self->next;
	pid_t ret = _Fork(copy_mem);
	int errno_save = errno;
	if (need_locks) {
		if (!ret) {
			for (pthread_t td=next; td!=self; td=td->next)
				td->tid = -1;
			if (__vmlock_lockptr) {
				__vmlock_lockptr[0] = 0;
				__vmlock_lockptr[1] = 0;
			}
		}
		__tl_unlock();
		__malloc_atfork(!ret);
		for (int i=0; i<sizeof atfork_locks/sizeof *atfork_locks; i++)
			if (*atfork_locks[i])
				if (ret) UNLOCK(*atfork_locks[i]);
				else **atfork_locks[i] = 0;
		__release_ptc();
		__ldso_atfork(!ret);
	}
#ifdef __wasilibc_unmodified_upstream
	__restore_sigs(&set);
#endif
	__fork_handler(!ret);
	if (ret<0) errno = errno_save;
	return ret;
}
