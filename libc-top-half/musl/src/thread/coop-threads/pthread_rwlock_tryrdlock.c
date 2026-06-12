#include "pthread_impl.h"

int __pthread_rwlock_tryrdlock(pthread_rwlock_t *rw)
{
	/* If no writer holds the lock, increment reader count */
	if (rw->_rw_lock >= 0) {
		rw->_rw_lock++;
		return 0;
	}
	return EBUSY;
}

weak_alias(__pthread_rwlock_tryrdlock, pthread_rwlock_tryrdlock);
