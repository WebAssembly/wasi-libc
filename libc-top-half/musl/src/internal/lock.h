#ifndef LOCK_H
#define LOCK_H

// Defines internal locking primitives. The main elements defined here are:
// - __lock_t: the type of a lock variable
// - __lock_ptr_t: the type of a pointer to a lock variable
// - __LOCK_INIT: initializer for a lock variable
// - STRONG_LOCK/STRONG_UNLOCK: functions to acquire/release a lock for a critical section that contains 
//                              potential context switch points, and thus must be executed even in cooperative threading mode.
// - WEAK_LOCK/WEAK_UNLOCK: functions to acquire/release a lock for a critical section that does not contain any context switch points, 
//                            and thus can be no-ops in cooperative threading mode.


#ifdef __wasi_cooperative_threads__
struct __waitlist_node;
struct __coop_lock {
	int owner;  // tid of owning thread, or 0 if unlocked
	struct __waitlist_node *waiters;
};

#define DECLARE_STRONG_LOCK(name, ...) __VA_ARGS__ struct __coop_lock name
#define DECLARE_WEAK_LOCK(name, ...)
#define __COOP_LOCK_INIT {0, NULL}
#define __LOCK_INIT __COOP_LOCK_INIT

hidden void __lock(struct __coop_lock *lock);
hidden void __unlock(struct __coop_lock *lock);
#define STRONG_LOCK(x) __lock(&x)
#define STRONG_UNLOCK(x) __unlock(&x)
#define WEAK_LOCK(x) ((void)0)
#define WEAK_UNLOCK(x) ((void)0)
#elif defined(_REENTRANT)
#define DECLARE_STRONG_LOCK(name, ...) __VA_ARGS__ volatile int name
#define DECLARE_WEAK_LOCK(name, ...) __VA_ARGS__ volatile int name
#define __LOCK_INIT {0}
hidden void __lock(volatile int *);
hidden void __unlock(volatile int *);
#define STRONG_LOCK(x) __lock(&x)
#define STRONG_UNLOCK(x) __unlock(&x)
#define WEAK_LOCK(x) __lock(&x)
#define WEAK_UNLOCK(x) __unlock(&x)
#else
#define DECLARE_STRONG_LOCK(name, ...)
#define DECLARE_WEAK_LOCK(name, ...)
#define __LOCK_INIT {0}
#define STRONG_LOCK(x) ((void)0)
#define STRONG_UNLOCK(x) ((void)0)
#define WEAK_LOCK(x) ((void)0)
#define WEAK_UNLOCK(x) ((void)0)
#endif

#endif
