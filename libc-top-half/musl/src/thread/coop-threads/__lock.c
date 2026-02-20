#include "pthread_impl.h"
#include "lock.h"
#include <wasi/api.h>

#ifndef __wasip3__
#error "Unknown WASI version"
#endif

void __lock(struct __coop_lock *lock)
{
	if (lock->owner == __pthread_self()->tid) {
		/* Trap on recursive locking. */
		__builtin_trap();
	}

	/* Loop until we acquire the lock. */
	while (lock->owner != 0) {
		__waitlist_wait_on(&lock->waiters);
		/* After waking, the lock might still be held by another
		 * thread that was scheduled before us, so loop back. */
	}
	
	lock->owner = __pthread_self()->tid;
}

void __unlock(struct __coop_lock *lock)
{
	int tid = __pthread_self()->tid;
	if (lock->owner != tid) {
		/* We're trying to unlock a lock we don't own. */
		__builtin_trap();
	}

	lock->owner = 0;
	/* Awake one waiter; the others will be resumed on future unlocks. */ 
	__waitlist_wake_one(&lock->waiters);
}