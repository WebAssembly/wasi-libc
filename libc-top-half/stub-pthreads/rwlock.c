#include "pthread_impl.h"
/* Musl uses bit31 to mark "has waiters", bit[30:0] all 1s to indicate writer */

/* These functions have the __ prefix to help stub out thread-specific data */

int __pthread_rwlock_rdlock(pthread_rwlock_t *rw)
{
	if (rw->_rw_lock == 0x7fffffff) return EDEADLK;
	if (rw->_rw_lock == 0x7ffffffe) return EAGAIN;
	rw->_rw_lock++;
	return 0;
}
weak_alias(__pthread_rwlock_rdlock, pthread_rwlock_rdlock);

int __pthread_rwlock_timedrdlock(pthread_rwlock_t *restrict rw, const struct timespec *restrict at)
{
	return pthread_rwlock_rdlock(rw);
}
weak_alias(__pthread_rwlock_timedrdlock, pthread_rwlock_timedrdlock);

int __pthread_rwlock_tryrdlock(pthread_rwlock_t *rw)
{
	if (rw->_rw_lock == 0x7fffffff) return EBUSY;
	if (rw->_rw_lock == 0x7ffffffe) return EAGAIN;
	rw->_rw_lock++;
	return 0;
}
weak_alias(__pthread_rwlock_tryrdlock, pthread_rwlock_tryrdlock);

int __pthread_rwlock_wrlock(pthread_rwlock_t *rw)
{
	if (rw->_rw_lock) return EDEADLK;
	rw->_rw_lock = 0x7fffffff;
	return 0;
}
weak_alias(__pthread_rwlock_wrlock, pthread_rwlock_wrlock);

int __pthread_rwlock_timedwrlock(pthread_rwlock_t *restrict rw, const struct timespec *restrict at)
{
	return pthread_rwlock_wrlock(rw);
}
weak_alias(__pthread_rwlock_timedwrlock, pthread_rwlock_timedwrlock);

int __pthread_rwlock_trywrlock(pthread_rwlock_t *rw)
{
	if (rw->_rw_lock) return EBUSY;
	rw->_rw_lock = 0x7fffffff;
	return 0;
}
weak_alias(__pthread_rwlock_trywrlock, pthread_rwlock_trywrlock);

int __pthread_rwlock_unlock(pthread_rwlock_t *rw)
{
	if (rw->_rw_lock == 0x7fffffff)
		rw->_rw_lock = 0;
	else
		rw->_rw_lock--;
	return 0;
}
weak_alias(__pthread_rwlock_unlock, pthread_rwlock_unlock);
