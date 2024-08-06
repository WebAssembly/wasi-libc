#include "pthread_impl.h"
/*
	Musl mutex (FYI)

	_m_type:
	b[7]	- process shared
	b[3]	- priority inherit
	b[2] 	- robust
	b[1:0] 	- type
		0 - normal
		1 - recursive
		2 - errorcheck

	_m_lock:
	b[30]	- owner dead, if robust
	b[29:0]	- tid, if not normal
	b[4]	- locked?, if normal
*/

int __pthread_mutex_consistent(pthread_mutex_t *m)
{
	/* cannot be a robust mutex, as they're entirely unsupported in WASI */
	return EINVAL;
}
weak_alias(__pthread_mutex_consistent, pthread_mutex_consistent);

int __pthread_mutex_lock(pthread_mutex_t *m)
{
	if (m->_m_type&3 != PTHREAD_MUTEX_RECURSIVE) {
		if (m->_m_count) return EDEADLK;
		m->_m_count = 1;
	} else {
		if ((unsigned)m->_m_count >= INT_MAX) return EAGAIN;
		m->_m_count++;
	}
	return 0;
}
weak_alias(__pthread_mutex_lock, pthread_mutex_lock);

int __pthread_mutex_trylock(pthread_mutex_t *m)
{
	if (m->_m_type&3 != PTHREAD_MUTEX_RECURSIVE) {
		if (m->_m_count) return EBUSY;
		m->_m_count = 1;
	} else {
		if ((unsigned)m->_m_count >= INT_MAX) return EAGAIN;
		m->_m_count++;
	}
	return 0;
}
weak_alias(__pthread_mutex_trylock, pthread_mutex_trylock);

int __pthread_mutex_unlock(pthread_mutex_t *m)
{
	if (!m->_m_count) return EPERM;
	m->_m_count--;
	return 0;
}
weak_alias(__pthread_mutex_unlock, pthread_mutex_unlock);

int __pthread_mutex_timedlock(pthread_mutex_t *restrict m, const struct timespec *restrict at)
{
	/* "The pthread_mutex_timedlock() function may fail if: A deadlock condition was detected." */
	/* This means that we don't have to wait and then return timeout, we can just detect deadlock. */
	return pthread_mutex_lock(m);
}
weak_alias(__pthread_mutex_timedlock, pthread_mutex_timedlock);
