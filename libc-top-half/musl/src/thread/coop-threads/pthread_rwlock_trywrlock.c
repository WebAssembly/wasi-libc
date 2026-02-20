#include "pthread_impl.h"

int __pthread_rwlock_trywrlock(pthread_rwlock_t *rw)
{
	int tid = wasip3_thread_index();

	/* Check for deadlock: trying to write-lock already owned write lock */
	if (rw->_rw_lock == -tid) {
		return EDEADLK;
	}

	/* Try to acquire write lock */
	if (rw->_rw_lock != 0) {
		return EBUSY;
	}

	rw->_rw_lock = -tid;
	return 0;
}
weak_alias(__pthread_rwlock_trywrlock, pthread_rwlock_trywrlock);
