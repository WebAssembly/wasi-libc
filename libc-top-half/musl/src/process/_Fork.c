#include <unistd.h>
#include <signal.h>
#include "syscall.h"
#ifdef __wasilibc_unmodified_upstream
#else
#include <wasi/api.h>
#endif
#include "libc.h"
#include "lock.h"
#include "pthread_impl.h"
#include "aio_impl.h"

static void dummy(int x) { }
weak_alias(dummy, __aio_atfork);

pid_t _Fork(int copy_mem)
{
	pid_t ret;
	sigset_t set;
	__block_all_sigs(&set);
	__aio_atfork(-1);
	LOCK(__abort_lock);
#ifdef __wasilibc_unmodified_upstream
#ifdef SYS_fork
	ret = __syscall(SYS_fork);
#else
	ret = __syscall(SYS_clone, SIGCHLD, 0);
#endif
#else
	__wasi_pid_t pid = -1;
    int err = __wasi_proc_fork(copy_mem, &pid);
	if (err != 0) {
		ret = -err;
	} else {
		ret = (int)pid;
	}
#endif
	if (!ret) {
		pthread_t self = __pthread_self();
#ifdef __wasilibc_unmodified_upstream
		self->tid = __syscall(SYS_gettid);
#else
		int r = __wasi_thread_id(&self->tid);
		if (r != 0) {
			/* Beyond this point should be unreachable. */
			a_crash();
			raise(SIGKILL);
			_Exit(127);
		}
#endif
		self->robust_list.off = 0;
		self->robust_list.pending = 0;
		self->next = self->prev = self;
		__thread_list_lock = 0;
		libc.threads_minus_1 = 0;
		if (libc.need_locks) libc.need_locks = -1;
	}
	UNLOCK(__abort_lock);
	__aio_atfork(!ret);
	__restore_sigs(&set);
	return __syscall_ret(ret);
}
